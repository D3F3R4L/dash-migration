/* Adapted from haraldott project
 * Author: fabioraraujo */
// - TCP Stream server and user-defined number of clients connected with an AP
// - WiFi connection
// - Tracing of throughput, packet information is done in the client

#include "ns3/point-to-point-helper.h"
#include <fstream>
#include "ns3/core-module.h"
#include <ns3/lte-module.h>
#include "ns3/applications-module.h"
#include "ns3/config-store.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/animation-interface.h"
#include "ns3/mobility-module.h"
#include <ns3/buildings-module.h>
#include "ns3/building-position-allocator.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "ns3/lte-helper.h"
#include "ns3/epc-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/tcp-stream-helper.h"
#include "ns3/tcp-stream-interface.h"
#include <iostream>

template <typename T>
std::string ToString(T val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
}

using namespace ns3;

void 
throughput(Ptr<FlowMonitor> flowMonitor,Ptr<Ipv4FlowClassifier> classifier)
{
  std::map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);

          NS_LOG_UNCOND("Flow ID: " << iter->first << " Src Addr " << t.sourceAddress << " Dst Addr " << t.destinationAddress);
          NS_LOG_UNCOND("Tx Packets = " << iter->second.txPackets);
          NS_LOG_UNCOND("Tx Bytes = " << iter->second.txBytes);
          //NS_LOG_UNCOND("Sum jitter = " << iter->second.jitterSum);
          //NS_LOG_UNCOND("Delay Sum = " << iter->second.delaySum);
          //NS_LOG_UNCOND("Lost Packet = " << iter->second.lostPackets);
          NS_LOG_UNCOND("Rx Bytes = " << iter->second.rxBytes);
          NS_LOG_UNCOND("Rx Packets = " << iter->second.rxPackets);
          //NS_LOG_UNCOND("timeLastRxPacket = " << iter->second.timeLastRxPacket.GetSeconds());
          //NS_LOG_UNCOND("timefirstTxPacket = " << iter->second.timeFirstTxPacket.GetSeconds());
          NS_LOG_UNCOND("Throughput: " << iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds()) / 1000/1000 << " Mbps");
          NS_LOG_UNCOND("Packet loss %= " << ((iter->second.txPackets-iter->second.rxPackets)*1.0)/iter->second.txPackets);
    }
  Simulator::Schedule(Seconds(1),&throughput,flowMonitor,classifier);
}

static void
funcaoDoida(ApplicationContainer clientApps, TcpStreamClientHelper clientHelper, Address server2Address, std::vector <std::pair <Ptr<Node>, std::string> > clients, uint16_t n)
{
  clientHelper.Handover(clientApps, clients.at (n).first, server2Address);
}

void 
stopSim (TcpStreamClientHelper clientHelper, NodeContainer UeNodes, uint32_t numberOfUeNodes)
{
  uint32_t closedApps = 0;
  closedApps = clientHelper.checkApps(UeNodes);
  if (closedApps>=numberOfUeNodes)
  {
    Simulator::Stop();
  }
  else
  {
    Simulator::Schedule(Seconds(5),&stopSim,clientHelper, UeNodes,numberOfUeNodes);    
  }
}

NS_LOG_COMPONENT_DEFINE ("dash-migrationExample");

int
main (int argc, char *argv[])
{
  uint16_t numberOfUeNodes = 24; // number of mobile devices
  uint16_t numberOfEnbNodes = 1; // number of ENBs
  uint64_t segmentDuration = 2000000;
  // The simulation id is used to distinguish log file results from potentially multiple consequent simulation runs.
  uint32_t simulationId = 1;
  uint32_t numberOfServers = 3;
  std::string adaptationAlgo = "festive";
  std::string segmentSizeFilePath = "contrib/dash/segmentSizes3.txt";

  CommandLine cmd;
  cmd.Usage ("Simulation of streaming with DASH.\n");
  cmd.AddValue ("numberOfUeNodes", "The number of clients (UE)", numberOfUeNodes);
  cmd.AddValue ("numberOfEnbNodes", "The number of ENB nodes", numberOfEnbNodes);
  cmd.AddValue ("simulationId", "The simulation's index (for logging purposes)", simulationId);
  cmd.AddValue ("segmentDuration", "The duration of a video segment in microseconds", segmentDuration);
  cmd.AddValue ("adaptationAlgo", "The adaptation algorithm that the client uses for the simulation", adaptationAlgo);
  cmd.AddValue ("segmentSizeFile", "The relative path (from ns-3.x directory) to the file containing the segment sizes in bytes", segmentSizeFilePath);

  cmd.Parse (argc, argv);

  Config::SetDefault ("ns3::LteUeNetDevice::DlEarfcn", UintegerValue (100));
  Config::SetDefault ("ns3::LteEnbNetDevice::DlEarfcn", UintegerValue (100));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue (18100));
  
  Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (50));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (50));

  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue (1446));
  Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue (524288));
  Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue (524288));

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults();

  // parse again so you can override default values from the command line
  Ptr<Node> pgw = epcHelper->GetPgwNode ();
  

   // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (numberOfServers);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  Ptr<Node> remoteHost2 = remoteHostContainer.Get (1);
  Ptr<Node> remoteHost3= remoteHostContainer.Get (2);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("35Mb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", StringValue ("50ms"));
  NetDeviceContainer server1 = p2ph.Install (remoteHost,pgw);

  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("35Mb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", StringValue ("60ms"));
  NetDeviceContainer server2 = p2ph.Install (remoteHost2,pgw);

  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("35Mb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", StringValue ("30ms"));
  NetDeviceContainer server3 = p2ph.Install (remoteHost3,pgw);

  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (server1);
  ipv4h.SetBase ("2.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces2 = ipv4h.Assign (server2);
  ipv4h.SetBase ("3.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces3 = ipv4h.Assign (server3);

  Ipv4Address server1Address = internetIpIfaces.GetAddress (0);
  Ipv4Address server2Address = internetIpIfaces2.GetAddress (0);
  Ipv4Address server3Address = internetIpIfaces3.GetAddress (0);
  NS_LOG_UNCOND(server1Address);
  NS_LOG_UNCOND(server2Address);
  NS_LOG_UNCOND(server3Address);
  //Ipv4StaticRoutingHelper ipv4RoutingHelper;
  /*for (uint32_t u = 0; u < numberOfServers; ++u)
    {
      Ptr<Node> remoteHost = remoteHostContainer.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
      remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);
    }
  */
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);
  Ptr<Ipv4StaticRouting> remoteStaticRouting2 = ipv4RoutingHelper.GetStaticRouting (remoteHost2->GetObject<Ipv4> ());
  remoteStaticRouting2->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);
  Ptr<Ipv4StaticRouting> remoteStaticRouting3 = ipv4RoutingHelper.GetStaticRouting (remoteHost3->GetObject<Ipv4> ());
  remoteStaticRouting3->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);
  /* Create Nodes */
  NodeContainer UeNodes;
  UeNodes.Create (numberOfUeNodes);

  NodeContainer EnbNodes;
  EnbNodes.Create (numberOfEnbNodes);

  /* Determin access point and server node */
 // Ptr<Node> apNode = UeNodes.Get (0);
 // Ptr<Node> serverNode = UeNodes.Get (1);

  /* Configure clients as STAs in the WLAN */
 // NodeContainer staContainer;
  /* Begin at +2, because position 0 is the access point and position 1 is the server */
 // for (NodeContainer::Iterator i = UeNodes.Begin (); i != UeNodes.End (); ++i)
   // {
     // staContainer.Add (*i);
   // }

  /* Determin client nodes for object creation with client helper class */
  std::vector <std::pair <Ptr<Node>, std::string> > clients;
  for (NodeContainer::Iterator i = UeNodes.Begin (); i != UeNodes.End (); ++i)
    {
      std::pair <Ptr<Node>, std::string> client (*i, adaptationAlgo);
      clients.push_back (client);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
//// Set up Building
//////////////////////////////////////////////////////////////////////////////////////////////////
  double roomHeight = 3;
  double roomLength = 6;
  double roomWidth = 5;
  uint32_t xRooms = 8;
  uint32_t yRooms = 3;
  uint32_t nFloors = 6;

  Ptr<Building> b = CreateObject <Building> ();
  b->SetBoundaries (Box ( 0.0, xRooms * roomWidth,
                          0.0, yRooms * roomLength,
                          0.0, nFloors * roomHeight));
  b->SetBuildingType (Building::Office);
  b->SetExtWallsType (Building::ConcreteWithWindows);
  b->SetNFloors (6);
  b->SetNRoomsX (8);
  b->SetNRoomsY (3);

  Vector posAp = Vector ( 1.0, 1.0, 1.0);
  // give the server node any position, it does not have influence on the simulation, it has to be set though,
  // because when we do: mobility.Install (UeNodes);, there has to be a position as place holder for the server
  // because otherwise the first client would not get assigned the desired position.
  Vector posServer = Vector (1.5, 1.5, 1.5);

  /* Set up positions of nodes (AP and server) */
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (posAp);
  positionAlloc->Add (posServer);


  Ptr<RandomRoomPositionAllocator> randPosAlloc = CreateObject<RandomRoomPositionAllocator> ();
  randPosAlloc->AssignStreams (simulationId);

  // create folder so we can log the positions of the clients
  const char * mylogsDir = dashLogDirectory.c_str();
  mkdir (mylogsDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  std::string tobascoDirTmp = dashLogDirectory + adaptationAlgo + "/";
  const char * tobascoDir = tobascoDirTmp.c_str();
  //const char * tobascoDir = (ToString (dashLogDirectory) + ToString (adaptationAlgo) + "/").c_str();
  mkdir (tobascoDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  std::string dirTmp = dashLogDirectory + adaptationAlgo + "/" + ToString (numberOfUeNodes) + "/";
  //const char * dir = (ToString (dashLogDirectory) + ToString (adaptationAlgo) + "/" + ToString (numberOfUeNodes) + "/").c_str();
  const char * dir = dirTmp.c_str();
  mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  std::cout << mylogsDir << "\n";
  std::cout << tobascoDir << "\n";
  std::cout << dir << "\n";

  std::ofstream clientPosLog;
  std::string clientPos = dashLogDirectory + adaptationAlgo + "/" + ToString (numberOfUeNodes) + "/" + "sim" + ToString (simulationId) + "_"  + "clientPos.txt";
  clientPosLog.open (clientPos.c_str());
  std::cout << clientPos << "\n";
  NS_ASSERT_MSG (clientPosLog.is_open(), "Couldn't open clientPosLog file");

  // allocate clients to positions
  for (uint i = 0; i < numberOfUeNodes; i++)
    {
      Vector pos = Vector (randPosAlloc->GetNext());
      positionAlloc->Add (pos);

      // log client positions
      clientPosLog << ToString(pos.x) << ", " << ToString(pos.y) << ", " << ToString(pos.z) << "\n";
      clientPosLog.flush ();
    }


  MobilityHelper mobility;
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (EnbNodes);
  BuildingsHelper::Install (EnbNodes);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (UeNodes);
  BuildingsHelper::Install (UeNodes);

  // Install LTE Devices to the nodes
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (EnbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (UeNodes);

  // Install the IP stack on the UEs
  internet.Install (UeNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));

  for (uint32_t u = 0; u < UeNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = UeNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  // Attach one UE per eNodeB
  lteHelper->Attach(ueLteDevs);

  uint16_t port= 9;

  /* Install TCP Receiver on the access point */
  TcpStreamServerHelper serverHelper (port); //NS_LOG_UNCOND("dash Install 277");
  ApplicationContainer serverApp = serverHelper.Install (remoteHostContainer); //NS_LOG_UNCOND("dash Install 278");
  //ApplicationContainer serverApp2 = serverHelper.Install (remoteHost2);
  //ApplicationContainer serverApp3 = serverHelper.Install (remoteHost3); 
  serverApp.Start (Seconds (1.0));
  //serverApp2.Start (Seconds (1.0));
  //serverApp3.Start (Seconds (1.0));

  std::vector <std::pair <Ptr<Node>, std::string> > clients_temp0;
  std::vector <std::pair <Ptr<Node>, std::string> > clients_temp1;
  std::vector <std::pair <Ptr<Node>, std::string> > clients_temp2;
  for (uint i = 0; i < numberOfUeNodes; i++)
    {
      if(i<numberOfUeNodes/3)
      {
        clients_temp0.push_back(clients[i]);
      }
      else 
        if(i<(2*numberOfUeNodes)/3)
          {
            clients_temp1.push_back(clients[i]);
          }
        else
          {
            clients_temp2.push_back(clients[i]);
          }
    }

  /* Install TCP/UDP Transmitter on the station */
  TcpStreamClientHelper clientHelper (server1Address, port);
  clientHelper.SetAttribute ("SegmentDuration", UintegerValue (segmentDuration));
  clientHelper.SetAttribute ("SegmentSizeFilePath", StringValue (segmentSizeFilePath));
  clientHelper.SetAttribute ("NumberOfClients", UintegerValue(numberOfUeNodes));
  clientHelper.SetAttribute ("SimulationId", UintegerValue (simulationId));
  clientHelper.SetAttribute ("ServerId", UintegerValue (0));
  ApplicationContainer clientApps = clientHelper.Install (clients_temp0);

  //TcpStreamClientHelper clientHelper2 (server2Address, port);
  clientHelper.SetAttribute ("RemoteAddress", AddressValue (server2Address));
  clientHelper.SetAttribute ("RemotePort", UintegerValue (port));
  clientHelper.SetAttribute ("SegmentDuration", UintegerValue (segmentDuration));
  clientHelper.SetAttribute ("SegmentSizeFilePath", StringValue (segmentSizeFilePath));
  clientHelper.SetAttribute ("NumberOfClients", UintegerValue(numberOfUeNodes));
  clientHelper.SetAttribute ("SimulationId", UintegerValue (simulationId));
  clientHelper.SetAttribute ("ServerId", UintegerValue (1));
  clientApps = clientHelper.Install (clients_temp1);

  //TcpStreamClientHelper clientHelper3 (server3Address, port);
  clientHelper.SetAttribute ("RemoteAddress", AddressValue (server3Address));
  clientHelper.SetAttribute ("RemotePort", UintegerValue (port));
  clientHelper.SetAttribute ("SegmentDuration", UintegerValue (segmentDuration));
  clientHelper.SetAttribute ("SegmentSizeFilePath", StringValue (segmentSizeFilePath));
  clientHelper.SetAttribute ("NumberOfClients", UintegerValue(numberOfUeNodes));
  clientHelper.SetAttribute ("SimulationId", UintegerValue (simulationId));
  clientHelper.SetAttribute ("ServerId", UintegerValue (2));
  clientApps = clientHelper.Install (clients_temp2);

  for (uint i = 0; i < clientApps.GetN (); i++)
    {
      double startTime = 2.0;
      clientApps.Get (i)->SetStartTime (Seconds (startTime+(i/100)));
    }/*
  for (uint i = 0; i < clientApps2.GetN (); i++)
    {
      double startTime = 2.0 + ((i * 3) / 100.0);
      clientApps2.Get (i)->SetStartTime (Seconds (startTime));
    }
  for (uint i = 0; i < clientApps3.GetN (); i++)
    {
      double startTime = 2.0 + ((i * 3) / 100.0);
      clientApps3.Get (i)->SetStartTime (Seconds (startTime));
    }*/

  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
  NS_LOG_INFO ("Run Simulation.");
  NS_LOG_INFO ("Sim: " << simulationId << "Clients: " << numberOfUeNodes);
  Simulator::Schedule(Seconds(5),&stopSim,clientHelper,UeNodes, numberOfUeNodes);
  //Simulator::Schedule(Seconds(1),&funcaoDoida,clientApps, clientHelper, server2Address, clients,0);
  //Simulator::Schedule(Seconds(1.05),&funcaoDoida,clientApps, clientHelper, server2Address, clients,9);
  //Simulator::Schedule(Seconds(1.1),&funcaoDoida,clientApps, clientHelper, server2Address, clients,10);
  //Simulator::Schedule(Seconds(1.15),&funcaoDoida,clientApps, clientHelper, server2Address, clients,11);
  //Simulator::Schedule(Seconds(1.2),&funcaoDoida,clientApps, clientHelper, server2Address, clients,12);
  //Simulator::Schedule(Seconds(1.25),&funcaoDoida,clientApps, clientHelper, server2Address, clients,13);
  //Simulator::Schedule(Seconds(1.3),&funcaoDoida,clientApps, clientHelper, server2Address, clients,14);
  //Simulator::Schedule(Seconds(1.35),&funcaoDoida,clientApps, clientHelper, server2Address, clients,15);
  //Simulator::Schedule(Seconds(1.4),&funcaoDoida,clientApps, clientHelper, server3Address, clients,16);
  //Simulator::Schedule(Seconds(1.45),&funcaoDoida,clientApps, clientHelper, server3Address, clients,17);
  //Simulator::Schedule(Seconds(1.5),&funcaoDoida,clientApps, clientHelper, server3Address, clients,18);
  //Simulator::Schedule(Seconds(1.55),&funcaoDoida,clientApps, clientHelper, server3Address, clients,19);
  //Simulator::Schedule(Seconds(1.6),&funcaoDoida,clientApps, clientHelper, server3Address, clients,20);
  //Simulator::Schedule(Seconds(1.65),&funcaoDoida,clientApps, clientHelper, server3Address, clients,21);
  //Simulator::Schedule(Seconds(1.7),&funcaoDoida,clientApps, clientHelper, server3Address, clients,22);
  //Simulator::Schedule(Seconds(1.75),&funcaoDoida,clientApps, clientHelper, server3Address, clients,23);
  //Simulator::Schedule(Seconds(1),&throughput,flowMonitor,classifier);
  Simulator::Run ();
  flowMonitor->SerializeToXmlFile ("results.xml" , true, true );
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

}

