///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

//Local dependencies
#include "TcpIpServerInterface.h"
#include "Multiplexor.h"
//STL
#include <iostream>
#include <sstream>

CTcpIpServerInterface::CTcpIpServerInterface(CInterfaceCallBack* picb,
    CInterfaceProperties ip): m_pssServer(0), CInterface(picb,ip)
{
}

CTcpIpServerSessionInterface::CTcpIpServerSessionInterface(CInterfaceCallBack* picb,
    CInterfaceProperties ip, wxSocketBase* psbConnection,std::vector<std::pair<std::string,CInterfaceProperties>> v):
CBlockDataInterface(picb,ip), m_psbConnection(psbConnection)
{
   m_ip.m_bSuspendOutput=false;
   if(m_ip.m_bOption1)
      for(int i=0;i<v.size();i++) m_sbuf.Add(v[i]);
   if(!m_psbConnection)
         throw std::string("CTcpIpServerInterface::OpenPipe accepted NULL connection");
   wxIPV4address addr;
   m_psbConnection->GetPeer(addr);
   char sz[20];
   itoa(addr.Service(),sz,10);
   g_ui.UpdateStatus("CTcpIpServerInterface::OpenPipe accepted a client "+std::string(addr.IPAddress().c_str())+":"+sz);
   m_ip.m_nPeerPort=addr.Service();
   m_ip.m_sPeerAddress=addr.IPAddress();
   m_ip.m_bReopenClosedPipe=false;
   m_ip.m_it=InterfaceType::TcpIpServerSession;
}

CTcpIpServerInterface::~CTcpIpServerInterface(void)
{
    if(m_pssServer) m_pssServer->Destroy();
    m_pssServer=0;
}

CTcpIpServerSessionInterface::~CTcpIpServerSessionInterface(void)
{
    if(m_psbConnection) m_psbConnection->Destroy();
    m_psbConnection=0;
}

std::pair<std::string,bool> CTcpIpServerInterface::GetString()
{
   return std::pair<std::string,bool>("",false);
}

std::pair<std::string,bool> CTcpIpServerSessionInterface::GetString()
{
    if(!BufferEmpty())
       return std::pair<std::string,bool>(GetStringFromBuffer(),true);
    if (!m_psbConnection->IsData())//no data to read
       return std::pair<std::string,bool>("",false);
    //rea data
    const int MAX_READ_BUFFER=1000;
    char sz[MAX_READ_BUFFER];
    m_psbConnection->Read(sz,MAX_READ_BUFFER-1);
    try
    {
      CheckErrors();
      sz[m_psbConnection->LastCount()]=0;
    }
    catch(std::string sError)
    {
       if (sError!="CTcpIpServerInterface::CheckErrors wxSOCKET_NOERROR") throw;
       //Peer disconnected
       sz[m_psbConnection->LastCount()]=0;
       m_psbConnection->Destroy();
       m_psbConnection=0;
    }
    AddData(sz);
    if(BufferEmpty())
       return std::pair<std::string,bool>("",false);
    else
       return std::pair<std::string,bool>(GetStringFromBuffer(),true);
}

void CTcpIpServerInterface::OpenPipe()
{
    if(m_pssServer && !m_pssServer->Ok())
    {//handle broken listening port
        m_pssServer->Destroy();
        m_pssServer=0; 
        g_ui.LogError("CTcpIpServerInterface::OpenPipe reset broken listining port");
    }
    if(m_pssServer==NULL)
    {//setup listening port
        wxIPV4address addr;// Create the address
        addr.Service(m_ip.m_nPort);
        // Create the socket and test if it's listening
        m_pssServer= new wxSocketServer(addr);
        if (!m_pssServer->Ok())
            throw std::string("CTcpIpServerInterface::OpenPipe failed to bind listener socket");
    }
   if (m_pssServer->WaitForAccept(1))//wait 1 second to accept a client
   {
      m_picb->AddTcpIpServerSessionInterface(new CTcpIpServerSessionInterface(m_picb,m_ip,m_pssServer->Accept(false),m_sbuf.GetQueue()));
      m_ip.m_bSuspendOutput=true;
      while(!m_sbuf.IsEmpty())m_sbuf.Get();
   }
}

void CTcpIpServerSessionInterface::OpenPipe()
{
   CBlockDataInterface::OpenPipe();
   if (CTcpIpServerSessionInterface::PipeClosed())
      throw std::string("CTcpIpServerSessionInterface::OpenPipe found closed pipe");
}

bool CTcpIpServerInterface::PipeClosed()
{
   return true;
}

bool CTcpIpServerSessionInterface::PipeClosed()
{
   if(m_psbConnection && m_psbConnection->IsConnected())
      return false;
   else 
      return true;
}

void CTcpIpServerInterface::Send(std::string sOut, std::string sInterfaceType)
{
   throw std::string("CTcpIpServerInterface::Send should not be called");
}

void CTcpIpServerSessionInterface::Send(std::string sOut, std::string sInterfaceType)
{
   if(m_psbConnection->WaitForWrite(1))//wait to write data
   {
      m_psbConnection->Write(sOut.c_str(),(int)sOut.length());
      CheckErrors();
      if (m_psbConnection->LastCount()!=sOut.length())
         throw std::string("CTcpIpServerInterface::Send wrong number of bites written");
   }
   else throw std::string("CTcpIpServerInterface::Send write timeout");
}

void CTcpIpServerInterface::CheckErrors()
{
   if(m_pssServer->Error())
   {
      std::ostringstream oss;
      oss<<m_pssServer->LastError();
      throw std::string("CTcpIpServerInterface::CheckErrors read/write error: "+oss.str());
   }
}

void CTcpIpServerSessionInterface::CheckErrors()
{
   if(m_psbConnection->Error())
   {
      if (m_psbConnection->LastError()==wxSOCKET_NOERROR)
         throw std::string("CTcpIpServerInterface::CheckErrors wxSOCKET_NOERROR");
      std::ostringstream oss;
      oss<<m_psbConnection->LastError();
      m_psbConnection->Destroy();
      m_psbConnection=0;
      throw std::string("CTcpIpServerSessionInterface::CheckErrors read/write error: "+oss.str());
   }
}
