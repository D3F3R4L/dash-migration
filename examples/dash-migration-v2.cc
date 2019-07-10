/* Adapted from haraldott project
 * Author: fabioraraujo */
// - TCP Stream server and user-defined number of clients connected with an AP
// - WiFi connection
// - Tracing of throughput, packet information is done in the client

#include "ns3/point-to-point-helper.h"
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include <ns3/buildings-module.h>
#include "ns3/building-position-allocator.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "ns3/flow-monitor-module.h"
#include "ns3/tcp-stream-helper.h"
#include "ns3/tcp-stream-interface.h"
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>

template <typename T>
std::string ToString(T val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
}

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("dash-migrationExample");

double StallMMESV1=0;
double RebufferMMESV1=0;
double StallMMESV2=0;
double RebufferMMESV2=0;
double StallMMESV3=0;
double RebufferMMESV3=0;
double StallMMECloud=0;
double RebufferMMECloud=0;
double StartMMESV1=0;
double StartMMESV2=0;
double StartMMESV3=0;
double StartMMECloud=0;
uint16_t n=3;
uint16_t MaxClientsSV=10;
uint32_t numberOfClients;
std::vector <uint32_t> SClients {0,0,0,0};
Address server1Address;
Address server2Address;
Address server3Address;
Address cloudAddress;

std::string dirTmp;
std::ofstream StallsLog;
std::ofstream RebufferLog;
std::ofstream StartTimeLog;
std::ofstream ServerScoreLog;

void
LogStall (uint16_t sv1,uint16_t sv2,uint16_t sv3,uint16_t cloud)
{
  StallsLog << std::setfill (' ') << std::setw (0) << Simulator::Now ().GetMicroSeconds ()  / (double)1000000 << ";"
              << std::setfill (' ') << std::setw (0) << sv1 << ";"
              << std::setfill (' ') << std::setw (0) << StallMMESV1 << ";"
              << std::setfill (' ') << std::setw (0) << sv2 << ";"
              << std::setfill (' ') << std::setw (0) << StallMMESV2 << ";"
              << std::setfill (' ') << std::setw (0) << sv3 << ";"
              << std::setfill (' ') << std::setw (0) << StallMMESV3 << ";"
              << std::setfill (' ') << std::setw (0) << cloud << ";"
              << std::setfill (' ') << std::setw (0) << StallMMECloud << ";\n";
  StallsLog.flush ();
}

void
LogRebuffer (double Tsv1,double Tsv2,double Tsv3,double Tcloud)
{
  RebufferLog << std::setfill (' ') << std::setw (0) << Simulator::Now ().GetMicroSeconds ()  / (double)1000000 << ";"
              << std::setfill (' ') << std::setw (0) << Tsv1 << ";"
              << std::setfill (' ') << std::setw (0) << RebufferMMESV1 << ";"
              << std::setfill (' ') << std::setw (0) << Tsv2 << ";"
              << std::setfill (' ') << std::setw (0) << RebufferMMESV2 << ";"
              << std::setfill (' ') << std::setw (0) << Tsv3 << ";"
              << std::setfill (' ') << std::setw (0) << RebufferMMESV3 << ";"
              << std::setfill (' ') << std::setw (0) << Tcloud << ";"
              << std::setfill (' ') << std::setw (0) << RebufferMMECloud << ";\n";
  RebufferLog.flush ();
}

void
LogStartTime (double sv1,double sv2,double sv3,double cloud)
{
  StartTimeLog << std::setfill (' ') << std::setw (0) << sv1 << ";"
              << std::setfill (' ') << std::setw (0) << StartMMESV1 << ";"
              << std::setfill (' ') << std::setw (0) << sv2 << ";"
              << std::setfill (' ') << std::setw (0) << StartMMESV2 << ";"
              << std::setfill (' ') << std::setw (0) << sv3 << ";"
              << std::setfill (' ') << std::setw (0) << StartMMESV3 << ";"
              << std::setfill (' ') << std::setw (0) << cloud << ";"
              << std::setfill (' ') << std::setw (0) << StartMMECloud << ";\n";
  StartTimeLog.flush ();
}
/*
void 
throughput(Ptr<FlowMonitor> flowMonitor,Ptr<Ipv4FlowClassifier> classifier)
{
  std::map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);
      if (iter->second.rxPackets<=10 and (t.sourceAddress=="1.0.0.1" or t.sourceAddress=="2.0.0.1" or t.sourceAddress=="3.0.0.1"))
      {
          NS_LOG_UNCOND("Flow ID: " << iter->first << " Src Addr " << t.sourceAddress << " Dst Addr " << t.destinationAddress);
          NS_LOG_UNCOND("Tx Packets = " << iter->second.txPackets);
          //NS_LOG_UNCOND("Tx Bytes = " << iter->second.txBytes);
          //NS_LOG_UNCOND("Sum jitter = " << iter->second.jitterSum);
          //NS_LOG_UNCOND("Delay Sum = " << iter->second.delaySum);
          //NS_LOG_UNCOND("Lost Packet = " << iter->second.lostPackets);
          //NS_LOG_UNCOND("Rx Bytes = " << iter->second.rxBytes);
          NS_LOG_UNCOND("Rx Packets = " << iter->second.rxPackets);
          //NS_LOG_UNCOND("timeLastRxPacket = " << iter->second.timeLastRxPacket.GetSeconds());
          //NS_LOG_UNCOND("timefirstTxPacket = " << iter->second.timeFirstTxPacket.GetSeconds());
         // NS_LOG_UNCOND("Throughput: " << iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds()) / 1000/1000 << " Mbps");
          //NS_LOG_UNCOND("Packet loss %= " << ((iter->second.txPackets-iter->second.rxPackets)*1.0)/iter->second.txPackets);
      }
    }
  Simulator::Schedule(Seconds(1),&throughput,flowMonitor,classifier);
}
*/
void
getThropughputClients(ApplicationContainer clientApps, TcpStreamClientHelper clientHelper, std::vector <std::pair <Ptr<Node>, std::string> > clients)
{
  for (uint i = 0; i < numberOfClients; i++)
  {
    clientHelper.GetThroughput(clientApps, clients.at (i).first);
  }
  Simulator::Schedule(Seconds(1),&getThropughputClients,clientApps,clientHelper,clients);
}

void
getThropughputServer(ApplicationContainer serverApp, TcpStreamServerHelper serverHelper,NodeContainer servers)
{
  for (uint j = 0; j < servers.GetN(); j++)
  {
    serverHelper.serverThroughput(serverApp, servers.Get(j));
  }
  Simulator::Schedule(Seconds(1),&getThropughputServer,serverApp, serverHelper,servers);
}

void
getStall(ApplicationContainer clientApps, TcpStreamClientHelper clientHelper, std::vector <std::pair <Ptr<Node>, std::string> > clients)
{
  uint16_t sv1=0;
  uint16_t sv2=0;
  uint16_t sv3=0;
  uint16_t cloud=0;
  double Tsv1=0;
  double Tsv2=0;
  double Tsv3=0;
  double Tcloud=0;

  for (uint i = 0; i < numberOfClients; i++)
  {
    std::string ip = clientHelper.GetServerAddress(clientApps, clients.at (i).first);
    if(ip=="1.0.0.1")
    {
      sv1+=clientHelper.GetNumbersOfBufferUnderrun(clientApps, clients.at (i).first);
      StallMMESV1=StallMMESV1 + (2*(sv1-StallMMESV1)/(n+1));
      Tsv1+=clientHelper.GetTotalBufferUnderrunTime(clientApps, clients.at (i).first);
      RebufferMMESV1=RebufferMMESV1 + (2*(Tsv1-RebufferMMESV1)/(n+1));
    }
    else
    {
      if(ip=="2.0.0.1")
      {
        sv2+=clientHelper.GetNumbersOfBufferUnderrun(clientApps, clients.at (i).first);
        StallMMESV2=StallMMESV2 + (2*(sv2-StallMMESV2)/(n+1));
        Tsv2+=clientHelper.GetTotalBufferUnderrunTime(clientApps, clients.at (i).first);
        RebufferMMESV2=RebufferMMESV2 + (2*(Tsv2-RebufferMMESV2)/(n+1));
      }
      else
      {
        if(ip=="3.0.0.1")
        {
          sv3+=clientHelper.GetNumbersOfBufferUnderrun(clientApps, clients.at (i).first);
          StallMMESV3=StallMMESV3 + (2*(sv3-StallMMESV3)/(n+1));
          Tsv3+=clientHelper.GetTotalBufferUnderrunTime(clientApps, clients.at (i).first);
          RebufferMMESV3=RebufferMMESV3 + (2*(Tsv3-RebufferMMESV3)/(n+1));
        }
        else
        {
          cloud+=clientHelper.GetNumbersOfBufferUnderrun(clientApps, clients.at (i).first);
          StallMMECloud=StallMMECloud + (2*(cloud-StallMMECloud)/(n+1));
          Tcloud+=clientHelper.GetTotalBufferUnderrunTime(clientApps, clients.at (i).first);
          RebufferMMECloud=RebufferMMECloud + (2*(Tcloud-RebufferMMECloud)/(n+1));
        }
      }
    }
  }
  LogStall(sv1,sv2,sv3,cloud);
  LogRebuffer(Tsv1,Tsv2,Tsv3,Tcloud);
  Simulator::Schedule(Seconds(1),&getStall,clientApps,clientHelper,clients);
}

static void
ServerHandover(ApplicationContainer clientApps, TcpStreamClientHelper clientHelper, Address server2Address, std::vector <std::pair <Ptr<Node>, std::string> > clients, uint16_t n)
{
  clientHelper.Handover(clientApps, clients.at (n).first, server2Address);
}

void
getStartTime(ApplicationContainer clientApps, TcpStreamClientHelper clientHelper, std::vector <std::pair <Ptr<Node>, std::string> > clients)
{
  double sv1=0;
  double sv2=0;
  double sv3=0;
  double cloud=0;
  for (uint i = 0; i < numberOfClients; i++)
  {
    std::string ip = clientHelper.GetServerAddress(clientApps, clients.at (i).first);
    if(ip=="1.0.0.1")
    {
      sv1+=clientHelper.GetPlaybakStartTime(clientApps, clients.at (i).first);
      StartMMESV1=StartMMESV1 + (2*(sv1-StartMMESV1)/(n+1));
    }
    else
    {
      if(ip=="2.0.0.1")
      {
        sv2+=clientHelper.GetPlaybakStartTime(clientApps, clients.at (i).first);
        StartMMESV2=StartMMESV2 + (2*(sv1-StartMMESV2)/(n+1));
      }
      else
      {
        if(ip=="3.0.0.1")
        {
          sv3+=clientHelper.GetPlaybakStartTime(clientApps, clients.at (i).first);
          StartMMESV3=StartMMESV3 + (2*(sv1-StartMMESV3)/(n+1));
        }
        else
        {
          cloud+=clientHelper.GetPlaybakStartTime(clientApps, clients.at (i).first);
          StartMMECloud=StartMMECloud + (2*(sv1-StartMMECloud)/(n+1));
        }
      }
    }
  }
  LogStartTime(sv1,sv2,sv3,cloud);
}

void
InitializeLogFiles (std::string dashLogDirectory, std::string m_algoName,std::string numberOfClients, std::string simulationId)
{
  NS_LOG_UNCOND("Inicializando log");
  std::string SLog = dashLogDirectory + m_algoName + "/" +  numberOfClients  + "/sim" + simulationId + "_" + "StallLog.csv";
  StallsLog.open (SLog.c_str ());
  StallsLog << "Time_Now;SV1_Stalls;SV1_Stalls_MME;SV2_Stalls;SV2_Stalls_MME;SV3_Stalls;SV3_Stalls_MME;Cloud_Stalls;Cloud_Stalls_MME\n";
  StallsLog.flush ();

  std::string RLog = dashLogDirectory + m_algoName + "/" +  numberOfClients  + "/sim" + simulationId + "_" + "RebufferLog.csv";
  RebufferLog.open (RLog.c_str ());
  RebufferLog << "Time_Now;SV1_Rebuffer;SV1_Rebuffer_MME;SV2_Rebuffer;SV2_Rebuffer_MME;SV3_Rebuffer;SV3_Rebuffer_MME;Cloud_Rebuffer;Cloud_Rebuffer_MME\n";
  RebufferLog.flush ();

  std::string STLog = dashLogDirectory + m_algoName + "/" +  numberOfClients  + "/sim" + simulationId + "_" + "PlaybackStartTime.csv";
  StartTimeLog.open (STLog.c_str ());
  StartTimeLog << "SV1_PlaybackStartTime;SV1_PlaybackStartTime_MME;SV2_PlaybackStartTime;SV2_PlaybackStartTime_MME;SV3_PlaybackStartTime;SV3_PlaybackStartTime_MME;Cloud_PlaybackStartTime;Cloud_PlaybackStartTime_MME\n";
  StartTimeLog.flush ();

  std::string SsLog = dashLogDirectory + m_algoName + "/" +  numberOfClients  + "/sim" + simulationId + "_" + "ServerScores.csv";
  ServerScoreLog.open (SsLog.c_str ());
  ServerScoreLog << "SV1_Score;SV2_Score;SV3_Score;Cloud_Score;\n";
  ServerScoreLog.flush ();

}

void 
stopSim (TcpStreamClientHelper clientHelper, NodeContainer staContainer)
{
  uint32_t closedApps = 0;
  closedApps = clientHelper.checkApps(staContainer);
  if (closedApps>=numberOfClients)
  {
    Simulator::Stop();
  }
  else
  {
    Simulator::Schedule(Seconds(5),&stopSim,clientHelper, staContainer);    
  }
}

std::string 
execute(const char* cmd) 
{
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
        if (result.size() > 0){
    result.resize(result.size()-1);
    }
    NS_LOG_UNCOND(result);
    return result;
}

std::vector <std::string>
split(const char *phrase, std::string delimiter){
    std::vector <std::string> list;
    std::string s = ToString (phrase);
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        list.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    list.push_back(s);
    return list;
}

void
getClientsOnServer(ApplicationContainer serverApp, TcpStreamServerHelper serverHelper,NodeContainer servers)
{
  for (uint j = 0; j < servers.GetN(); j++)
  {
    SClients[j]=serverHelper.NumberOfClients(serverApp, servers.Get(j));
    NS_LOG_UNCOND(SClients[j]);
  }
}

void
politica(ApplicationContainer clientApps, TcpStreamClientHelper clientHelper, std::vector <std::pair <Ptr<Node>, std::string> > clients,ApplicationContainer serverApp, TcpStreamServerHelper serverHelper,NodeContainer servers)
{
  getClientsOnServer(serverApp, serverHelper, servers);
  std::string filename = "python3 src/dash-migration/AHP/AHP.py " + dirTmp;
  std::string bestSv = execute(filename.c_str());
  //std::string bestSv="1.0.0.1 2.0.0.1 3.0.0.1";
  //system(filename.c_str());
  std::vector <std::string> BestServers;
  BestServers = split(bestSv.c_str(), " ");

  for (uint i = 0; i < numberOfClients; i++)
  {
    std::string ip = clientHelper.GetServerAddress(clientApps, clients.at (i).first);
    for (uint j = 0; j < BestServers.size(); j++)
    {
      Address SvIp;
      uint32_t SvClients;
      uint16_t aux;
      switch(BestServers[j].at(0))
      {
        case '1':
          SvIp=server1Address;
          SvClients=SClients[0];
          aux=0;
          break;
        case '2':
          SvIp=server2Address;
          SvClients=SClients[1];
          aux=1;
          break;
        case '3':
          SvIp=server3Address;
          SvClients=SClients[2];
          aux=2;
          break;
      }
      if (ip==BestServers[j])
      {
        j=BestServers.size();
      }
      else
      {
        if(SvClients<MaxClientsSV)
        {
          std::cout << SvIp << "ServerId: \t" << i << " Cliente" << SClients[aux]<< std::endl;
          SClients[aux]=SClients[aux]+1;
          ServerHandover(clientApps, clientHelper, SvIp, clients,i);
          j=BestServers.size();
        }
      }
    }
  }
  Simulator::Schedule(Seconds(2),&politica,clientApps,clientHelper,clients,serverApp, serverHelper,servers);
}

void
politica2(ApplicationContainer clientApps, TcpStreamClientHelper clientHelper, std::vector <std::pair <Ptr<Node>, std::string> > clients,ApplicationContainer serverApp, TcpStreamServerHelper serverHelper,NodeContainer servers)
{
  Address SvIp;
  uint16_t aux;
  for (uint i = 0; i < numberOfClients; i++)
  {
    std::string ip = clientHelper.GetServerAddress(clientApps, clients.at (i).first);
    getClientsOnServer(serverApp, serverHelper, servers);
    std::string filename = "python3 src/dash-migration/Guloso-Aleatorio/exemplo.py " + dirTmp +" ale "+ ToString(SClients[0])+" "+ ToString(SClients[1])+" "+ ToString(SClients[2])+" "+ ToString(SClients[3])+" "+ip;
    std::string bestSv = execute(filename.c_str());
    //std::string bestSv="1.0.0.1 2.0.0.1 3.0.0.1";
    //system(filename.c_str());
    std::vector <std::string> BestServers;
    BestServers = split(bestSv.c_str(), " ");
    switch(BestServers[0].at(0))
    {
      case '1':
        SvIp=server1Address;
        aux=0;
        break;
      case '2':
        SvIp=server2Address;
        aux=1;
        break;
      case '3':
        SvIp=server3Address;
        aux=2;
        break;
    }
    if (ip!=BestServers[0])
    {
      std::cout << SvIp << "ServerId: \t" << i << " Cliente" << SClients[aux]<< std::endl;
      ServerHandover(clientApps, clientHelper, SvIp, clients,i);
    }
  }
  Simulator::Schedule(Seconds(2),&politica2,clientApps,clientHelper,clients,serverApp, serverHelper,servers);
}

int
main (int argc, char *argv[])
{

  uint64_t segmentDuration = 2000000;
  // The simulation id is used to distinguish log file results from potentially multiple consequent simulation runs.
  uint32_t simulationId = 1;
  numberOfClients = 4;
  uint32_t numberOfServers = 5;
  std::string adaptationAlgo = "festive";
  std::string segmentSizeFilePath = "src/dash-migration/dash/segmentSizesBigBuck1A.txt";
  bool shortGuardInterval = true;

  //lastRx=[numberOfClients];

  CommandLine cmd;
  cmd.Usage ("Simulation of streaming with DASH.\n");
  cmd.AddValue ("simulationId", "The simulation's index (for logging purposes)", simulationId);
  cmd.AddValue ("numberOfClients", "The number of clients", numberOfClients);
  cmd.AddValue ("segmentDuration", "The duration of a video segment in microseconds", segmentDuration);
  cmd.AddValue ("adaptationAlgo", "The adaptation algorithm that the client uses for the simulation", adaptationAlgo);
  cmd.AddValue ("segmentSizeFile", "The relative path (from ns-3.x directory) to the file containing the segment sizes in bytes", segmentSizeFilePath);
  cmd.Parse (argc, argv);


  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue (1446));
  Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue (524288));
  Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue (524288));

  WifiHelper wifiHelper;
  wifiHelper.SetStandard (WIFI_PHY_STANDARD_80211n_5GHZ);
  wifiHelper.SetRemoteStationManager ("ns3::MinstrelHtWifiManager");//


  /* Set up Legacy Channel */
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  // We do not set an explicit propagation loss model here, we just use the default ones that get applied with the building model.

  /* Setup Physical Layer */
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
  wifiPhy.Set ("TxPowerStart", DoubleValue (20.0));//
  wifiPhy.Set ("TxPowerEnd", DoubleValue (20.0));//
  wifiPhy.Set ("TxPowerLevels", UintegerValue (1));//
  wifiPhy.Set ("TxGain", DoubleValue (0));//
  wifiPhy.Set ("RxGain", DoubleValue (0));//
  wifiPhy.SetErrorRateModel ("ns3::YansErrorRateModel");//
  wifiPhy.SetChannel (wifiChannel.Create ());
  wifiPhy.Set("ShortGuardEnabled", BooleanValue(shortGuardInterval));
  wifiPhy.Set ("Antennas", UintegerValue (4));

  /* Create Nodes */
  NodeContainer networkNodes;
  networkNodes.Create (numberOfClients + numberOfServers);

  /* Determin access point and server node */
  Ptr<Node> apNode = networkNodes.Get (0);
  Ptr<Node> serverNode = networkNodes.Get (1);
  Ptr<Node> serverNode2 = networkNodes.Get (2);
  Ptr<Node> serverNode3 = networkNodes.Get (3);
  Ptr<Node> cloudNode = networkNodes.Get (4);

  /* Configure clients as STAs in the WLAN */
  NodeContainer staContainer;
  /* Begin at +2, because position 0 is the access point and position 1 is the server */
  for (NodeContainer::Iterator i = networkNodes.Begin () + numberOfServers; i != networkNodes.End (); ++i)
    {
      staContainer.Add (*i);
    }

  /* Determin client nodes for object creation with client helper class */
  std::vector <std::pair <Ptr<Node>, std::string> > clients;
  for (NodeContainer::Iterator i = networkNodes.Begin () + numberOfServers; i != networkNodes.End (); ++i)
    {
      std::pair <Ptr<Node>, std::string> client (*i, adaptationAlgo);
      clients.push_back (client);
    }

  /* Set up WAN link between server node and access point*/
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("1Gb/s")); // This must not be more than the maximum throughput in 802.11n
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2p.SetChannelAttribute ("Delay", StringValue ("11ms"));
  NetDeviceContainer wanIpDevices;
  wanIpDevices = p2p.Install (serverNode, apNode);

  p2p.SetDeviceAttribute ("DataRate", StringValue ("1Gb/s")); // This must not be more than the maximum throughput in 802.11n
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2p.SetChannelAttribute ("Delay", StringValue ("12ms"));
  NetDeviceContainer wanIpDevices2;
  wanIpDevices2 = p2p.Install (serverNode2, serverNode);

  p2p.SetDeviceAttribute ("DataRate", StringValue ("1Gb/s")); // This must not be more than the maximum throughput in 802.11n
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2p.SetChannelAttribute ("Delay", StringValue ("11ms"));
  NetDeviceContainer wanIpDevices3;
  wanIpDevices3 = p2p.Install (serverNode3, serverNode2);

  p2p.SetDeviceAttribute ("DataRate", StringValue ("1Gb/s")); // This must not be more than the maximum throughput in 802.11n
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2p.SetChannelAttribute ("Delay", StringValue ("96ms"));
  NetDeviceContainer wanIpDevices4;
  wanIpDevices4 = p2p.Install (cloudNode, serverNode3);

  /* create MAC layers */
  WifiMacHelper wifiMac;
  /* WLAN configuration */
  Ssid ssid = Ssid ("network");
  /* Configure STAs for WLAN*/

  wifiMac.SetType ("ns3::StaWifiMac",
                    "Ssid", SsidValue (ssid));
  NetDeviceContainer staDevices;
  staDevices = wifiHelper.Install (wifiPhy, wifiMac, staContainer);

  /* Configure AP for WLAN*/
  wifiMac.SetType ("ns3::ApWifiMac",
                    "Ssid", SsidValue (ssid));
  NetDeviceContainer apDevice;
  apDevice = wifiHelper.Install (wifiPhy, wifiMac, apNode);



  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/ChannelWidth", UintegerValue (80));

  /* Determin WLAN devices (AP and STAs) */
  NetDeviceContainer wlanDevices;
  wlanDevices.Add (staDevices);
  wlanDevices.Add (apDevice);

  /* Internet stack */
  InternetStackHelper stack;
  stack.Install (networkNodes);

  /* Assign IP addresses */
  Ipv4AddressHelper address;
  Ipv4AddressHelper address2;
  Ipv4AddressHelper address3;
  Ipv4AddressHelper address4;

  /* IPs for WAN */
  address.SetBase ("1.0.0.0", "255.255.255.0");
  address2.SetBase ("2.0.0.0", "255.255.255.0");
  address3.SetBase ("3.0.0.0", "255.255.255.0");
  address4.SetBase ("4.0.0.0", "255.255.255.0");
  Ipv4InterfaceContainer wanInterface = address.Assign (wanIpDevices);
  Ipv4InterfaceContainer wanInterface2 = address2.Assign (wanIpDevices2);
  Ipv4InterfaceContainer wanInterface3 = address3.Assign (wanIpDevices3);
  Ipv4InterfaceContainer wanInterface4 = address4.Assign (wanIpDevices4);
  

server1Address = Address(wanInterface.GetAddress (0));
server2Address = Address(wanInterface2.GetAddress (0));
server3Address = Address(wanInterface3.GetAddress (0));
cloudAddress = Address(wanInterface4.GetAddress (0));

  /* IPs for WLAN (STAs and AP) */
  address.SetBase ("192.168.1.0", "255.255.255.0");
  address.Assign (wlanDevices);

  /* Populate routing table */
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  uint16_t port = 9;


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
  // because when we do: mobility.Install (networkNodes);, there has to be a position as place holder for the server
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
  dirTmp = dashLogDirectory + adaptationAlgo + "/" + ToString (numberOfClients) + "/";
  //const char * dir = (ToString (dashLogDirectory) + ToString (adaptationAlgo) + "/" + ToString (numberOfClients) + "/").c_str();
  const char * dir = dirTmp.c_str();
  mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  std::cout << mylogsDir << "\n";
  std::cout << tobascoDir << "\n";
  std::cout << dir << "\n";

  std::ofstream clientPosLog;
  std::string clientPos = dashLogDirectory + adaptationAlgo + "/" + ToString (numberOfClients) + "/" + "sim" + ToString (simulationId) + "_"  + "clientPos.txt";
  clientPosLog.open (clientPos.c_str());
  std::cout << clientPos << "\n";
  NS_ASSERT_MSG (clientPosLog.is_open(), "Couldn't open clientPosLog file");

  // allocate clients to positions
  for (uint i = 0; i < numberOfClients; i++)
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
  // install all of the nodes that have been added to positionAlloc to the mobility model
  mobility.Install (networkNodes);
  BuildingsHelper::Install (networkNodes); // networkNodes contains all nodes, stations and ap
  BuildingsHelper::MakeMobilityModelConsistent ();

  // if logging of the packets between AP---Server or AP and the STAs is wanted, these two lines can be activated

  // p2p.EnablePcapAll ("p2p-", true);
  // wifiPhy.EnablePcapAll ("wifi-", true);

  NodeContainer servers;
  servers.Add(serverNode);
  servers.Add(serverNode2);
  servers.Add(serverNode3);
  servers.Add(cloudNode);
  TcpStreamServerHelper serverHelper (port,simulationId,dirTmp); //NS_LOG_UNCOND("dash Install 277");
  serverHelper.SetAttribute ("RemoteAddress", AddressValue (server1Address));
  ApplicationContainer serverApp = serverHelper.Install (serverNode);//NS_LOG_UNCOND("dash Install 278");
  serverHelper.SetAttribute ("RemoteAddress", AddressValue (server2Address));
  serverApp = serverHelper.Install (serverNode2);
  serverHelper.SetAttribute ("RemoteAddress", AddressValue (server3Address));
  serverApp = serverHelper.Install (serverNode3);
  serverHelper.SetAttribute ("RemoteAddress", AddressValue (cloudAddress));
  serverApp = serverHelper.Install (cloudNode);
  serverApp.Start (Seconds (1.0));
  //serverApp2.Start (Seconds (1.0));
  //serverApp3.Start (Seconds (1.0));
  std::vector <std::pair <Ptr<Node>, std::string> > clients_temp0;
  std::vector <std::pair <Ptr<Node>, std::string> > clients_temp1;
  std::vector <std::pair <Ptr<Node>, std::string> > clients_temp2;
  std::vector <std::pair <Ptr<Node>, std::string> > clients_temp3;
  
  for (uint i = 0; i < numberOfClients; i++)
    {
      if(i<numberOfClients/4)
      {
        clients_temp0.push_back(clients[i]);
      }
      else 
        if(i<(2*numberOfClients)/4)
          {
            clients_temp1.push_back(clients[i]);
          }
        else
          if(i<(3*numberOfClients)/4)
          {
            clients_temp2.push_back(clients[i]);
          }
          else
          {
            clients_temp3.push_back(clients[i]);
          }
    }

  /* Install TCP/UDP Transmitter on the station */
  TcpStreamClientHelper clientHelper (server1Address, port);
  clientHelper.SetAttribute ("SegmentDuration", UintegerValue (segmentDuration));
  clientHelper.SetAttribute ("SegmentSizeFilePath", StringValue (segmentSizeFilePath));
  clientHelper.SetAttribute ("NumberOfClients", UintegerValue(numberOfClients));
  clientHelper.SetAttribute ("SimulationId", UintegerValue (simulationId));
  clientHelper.SetAttribute ("ServerId", UintegerValue (0));
  ApplicationContainer clientApps = clientHelper.Install (clients_temp0);

  //TcpStreamClientHelper clientHelper2 (server2Address, port);
  clientHelper.SetAttribute ("RemoteAddress", AddressValue (server2Address));
  clientHelper.SetAttribute ("RemotePort", UintegerValue (port));
  clientHelper.SetAttribute ("SegmentDuration", UintegerValue (segmentDuration));
  clientHelper.SetAttribute ("SegmentSizeFilePath", StringValue (segmentSizeFilePath));
  clientHelper.SetAttribute ("NumberOfClients", UintegerValue(numberOfClients));
  clientHelper.SetAttribute ("SimulationId", UintegerValue (simulationId));
  clientHelper.SetAttribute ("ServerId", UintegerValue (1));
  clientApps = clientHelper.Install (clients_temp1);

  //TcpStreamClientHelper clientHelper3 (server3Address, port);
  clientHelper.SetAttribute ("RemoteAddress", AddressValue (server3Address));
  clientHelper.SetAttribute ("RemotePort", UintegerValue (port));
  clientHelper.SetAttribute ("SegmentDuration", UintegerValue (segmentDuration));
  clientHelper.SetAttribute ("SegmentSizeFilePath", StringValue (segmentSizeFilePath));
  clientHelper.SetAttribute ("NumberOfClients", UintegerValue(numberOfClients));
  clientHelper.SetAttribute ("SimulationId", UintegerValue (simulationId));
  clientHelper.SetAttribute ("ServerId", UintegerValue (2));
  clientApps = clientHelper.Install (clients_temp2);

  clientHelper.SetAttribute ("RemoteAddress", AddressValue (cloudAddress));
  clientHelper.SetAttribute ("RemotePort", UintegerValue (port));
  clientHelper.SetAttribute ("SegmentDuration", UintegerValue (segmentDuration));
  clientHelper.SetAttribute ("SegmentSizeFilePath", StringValue (segmentSizeFilePath));
  clientHelper.SetAttribute ("NumberOfClients", UintegerValue(numberOfClients));
  clientHelper.SetAttribute ("SimulationId", UintegerValue (simulationId));
  clientHelper.SetAttribute ("ServerId", UintegerValue (3));
  clientApps = clientHelper.Install (clients_temp3);
  
  for (uint i = 0; i < clientApps.GetN (); i++)
    {
      double startTime = 2.0;
      clientApps.Get (i)->SetStartTime (Seconds (startTime+(i/100)));
    }
/*

  /* Install TCP Receiver on the access point */
/*
  std::vector <std::pair <Ptr<Node>, std::string> > clients_temp0;
  clients_temp0.push_back(clients[0]);*/

  /* Install TCP/UDP Transmitter on the station */
  //TcpStreamClientHelper clientHelper (serverAddress, port); //NS_LOG_UNCOND("dash Install 288");
  //clientHelper.SetAttribute ("SegmentDuration", UintegerValue (segmentDuration));
  //clientHelper.SetAttribute ("SegmentSizeFilePath", StringValue (segmentSizeFilePath));
  //clientHelper.SetAttribute ("NumberOfClients", UintegerValue(numberOfClients));
  //clientHelper.SetAttribute ("SimulationId", UintegerValue (simulationId)); //NS_LOG_UNCOND("dash Install 292");
  //ApplicationContainer clientApps = clientHelper.Install (clients); //NS_LOG_UNCOND("dash Install 293");


/*
  std::vector <std::pair <Ptr<Node>, std::string> > clients_temp1;
  clients_temp1.push_back(clients[1]);

  TcpStreamClientHelper clientHelper2 (serverAddress2, port);
  clientHelper2.SetAttribute ("SegmentDuration", UintegerValue (segmentDuration));
  clientHelper2.SetAttribute ("SegmentSizeFilePath", StringValue (segmentSizeFilePath));
  clientHelper2.SetAttribute ("NumberOfClients", UintegerValue(numberOfClients));
  clientHelper2.SetAttribute ("SimulationId", UintegerValue (simulationId+1));
  ApplicationContainer clientApps2 = clientHelper2.Install (clients_temp1);*/
  //for (uint i = 0; i < clientApps.GetN (); i++)
    //{
      //double startTime = 2.0 + ((i * 3) / 100.0);
      //clientApps.Get (i)->SetStartTime (Seconds (startTime));
    //}
  /*
  for (uint i = 0; i < clientApps2.GetN (); i++)
    {
      double startTime = 2.0 + ((i * 3) / 100.0);
      clientApps2.Get (i)->SetStartTime (Seconds (startTime)); 
    }*/

  //Ptr<FlowMonitor> flowMonitor;
  //FlowMonitorHelper flowHelper;
  //flowMonitor = flowHelper.InstallAll();
  //Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
  
  InitializeLogFiles (dashLogDirectory, adaptationAlgo,ToString(numberOfClients),ToString(simulationId));

  NS_LOG_INFO ("Run Simulation.");
  NS_LOG_INFO ("Sim: " << simulationId << "Clients: " << numberOfClients);
  //NS_LOG_UNCOND("SERVER1"<< serverAddress);
  //NS_LOG_UNCOND("SERVER2"<< serverAddress2);
  //Simulator::Schedule(Seconds(5),&ServerHandover,clientApps, clientHelper, server2Address, clients,0);
  //Simulator::Schedule(Seconds(1.05),&ServerHandover,clientApps, clientHelper, server2Address, clients,1);
  //Simulator::Schedule(Seconds(1.1),&ServerHandover,clientApps, clientHelper, server2Address, clients,2);
  //Simulator::Schedule(Seconds(1.15),&ServerHandover,clientApps, clientHelper, server2Address, clients,3);
  //Simulator::Schedule(Seconds(1.2),&ServerHandover,clientApps, clientHelper, server2Address, clients,4);
  //Simulator::Schedule(Seconds(1),&ServerHandover,clientApps, clientHelper, server3Address, clients,5);
  //Simulator::Schedule(Seconds(1.05),&ServerHandover,clientApps, clientHelper, server3Address, clients,6);
  //Simulator::Schedule(Seconds(1.1),&ServerHandover,clientApps, clientHelper, server3Address, clients,7);
  //Simulator::Schedule(Seconds(1.15),&ServerHandover,clientApps, clientHelper, server3Address, clients,8);
  //Simulator::Schedule(Seconds(1.2),&ServerHandover,clientApps, clientHelper, server3Address, clients,9);
  Simulator::Schedule(Seconds(2),&getThropughputServer,serverApp, serverHelper,servers);
  Simulator::Schedule(Seconds(2),&getThropughputClients,clientApps,clientHelper,clients);
  Simulator::Schedule(Seconds(3),&getStall,clientApps,clientHelper,clients);
  //Simulator::Schedule(Seconds(5),&politica,clientApps,clientHelper,clients,serverApp, serverHelper,servers);
  Simulator::Schedule(Seconds(5),&politica2,clientApps,clientHelper,clients,serverApp, serverHelper,servers);
  Simulator::Schedule(Seconds(5),&stopSim,clientHelper,staContainer);
  Simulator::Schedule(Seconds(10),&getStartTime,clientApps,clientHelper,clients);
  //Simulator::Schedule(Seconds(1),&throughput,flowMonitor,classifier);
  Simulator::Run ();
  //flowMonitor->SerializeToXmlFile ("results.xml" , true, true );
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

}
