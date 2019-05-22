/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#include "tcp-stream-helper.h"
#include "ns3/tcp-stream-server.h"
#include "ns3/tcp-stream-client.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"

namespace ns3 {

TcpStreamServerHelper::TcpStreamServerHelper (uint16_t port)
{
  m_factory.SetTypeId (TcpStreamServer::GetTypeId ());
  SetAttribute ("Port", UintegerValue (port));
}

void
TcpStreamServerHelper::SetAttribute (
  std::string name,
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
TcpStreamServerHelper::Install (Ptr<Node> node) const
{NS_LOG_UNCOND("serverhelper Install 44");
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
TcpStreamServerHelper::Install (std::string nodeName) const
{NS_LOG_UNCOND("serverhelper Install 50");
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
TcpStreamServerHelper::Install (NodeContainer c) const
{NS_LOG_UNCOND("serverhelper Install 57");
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
TcpStreamServerHelper::InstallPriv (Ptr<Node> node) const
{ NS_LOG_UNCOND("serverhelper InstallPriv 69");
  Ptr<Application> app = m_factory.Create<TcpStreamServer> ();
  node->AddApplication (app);

  return app;
}

TcpStreamClientHelper::TcpStreamClientHelper (Address address, uint16_t port)
{ NS_LOG_UNCOND("clienthelper construtor 77");
  m_factory.SetTypeId (TcpStreamClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (address));
  SetAttribute ("RemotePort", UintegerValue (port));
}

TcpStreamClientHelper::TcpStreamClientHelper (Ipv4Address address, uint16_t port)
{ NS_LOG_UNCOND("clienthelper construtor 84");
  m_factory.SetTypeId (TcpStreamClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (Address(address)));
  SetAttribute ("RemotePort", UintegerValue (port));
}

TcpStreamClientHelper::TcpStreamClientHelper (Ipv6Address address, uint16_t port)
{ NS_LOG_UNCOND("clienthelper construtor 91");
  m_factory.SetTypeId (TcpStreamClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (Address(address)));
  SetAttribute ("RemotePort", UintegerValue (port));
}

void
TcpStreamClientHelper::SetAttribute (std::string name, const AttributeValue &value)
{ //NS_LOG_UNCOND("clienthelper setAttribute 99");
  m_factory.Set (name, value);
}

ApplicationContainer
TcpStreamClientHelper::Install (std::vector <std::pair <Ptr<Node>, std::string> > clients) const
{ NS_LOG_UNCOND("clienthelper Install 105");
  ApplicationContainer apps;
  for (uint i = 0; i < clients.size (); i++)
    {
      apps.Add (InstallPriv (clients.at (i).first, clients.at (i).second, i));
    }

  return apps;
}

void
//TcpStreamClientHelper::Handover(Ptr<Node> node, ApplicationContainer node, Address ip)
TcpStreamClientHelper::Handover(ApplicationContainer clientApps, Ptr<Node> node, Address ip)
{ NS_LOG_UNCOND("clientHelper Handover 118");
  //Ptr<Application> app = m_factory.Create<TcpStreamClient> ();
  Ptr<Application> app = node->GetApplication(0);
  app->GetObject<TcpStreamClient> ()->HandoverApplication (ip);
  //app->GetObject<TcpStreamClient> ()->StopApplication();

  //Ptr<Node> node
  //TcpStreamClient::HandoverApplication ();
/*
  //ApplicationContainer apps;
  Ptr<Application> app = m_factory.Create<TcpStreamClient> ();
  app->GetObject<TcpStreamClient> ()-> HandoverApplication(ip);
  //node->GetObject<TcpStreamClient> ()->HandoverApplication (ip);
  //TcpStreamClient::HandoverApplication (ip);
  //HandoverApplication (ip);*/
      
}

Ptr<Application>
TcpStreamClientHelper::InstallPriv (Ptr<Node> node, std::string algo, uint16_t clientId) const
{ NS_LOG_UNCOND("clienthelper InstallPriv 130");
  Ptr<Application> app = m_factory.Create<TcpStreamClient> ();
  app->GetObject<TcpStreamClient> ()->SetAttribute ("ClientId", UintegerValue (clientId));
  app->GetObject<TcpStreamClient> ()->Initialise (algo, clientId);
  node->AddApplication (app); 
  //app->GetObject<TcpStreamClient> ()->SetRemote (Ipv4Address ip, uint16_t port);
  return app;
}

uint32_t
TcpStreamClientHelper::checkApps(NodeContainer staContainer, uint32_t closedApps)
{ NS_LOG_UNCOND("checkApps");
  bool c;
  uint32_t nNodes = staContainer.GetN ();
  for (uint32_t i = 0; i < nNodes; ++i)
  {
    Ptr<Node> p = staContainer.Get (i);
    Ptr<Application> app = p->GetApplication(0);
    c=app->GetObject<TcpStreamClient> ()->check ();
    //NS_LOG_UNCOND(c);
    if (c==true)
    {
      closedApps++;
    }
  }
  return closedApps;
}

} // namespace ns3
