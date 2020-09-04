///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

//Local dependencies
#include "TcpIpClientInterface.h"

//STL
#include <iostream>
#include <sstream>

//wxWidgets
#include "wx/utils.h"

CTcpIpClientInterface::CTcpIpClientInterface(CInterfaceCallBack* picb,
    CInterfaceProperties ip) : CBlockDataInterface(picb,ip), m_pscClient(0)
{
}

CTcpIpClientInterface::~CTcpIpClientInterface(void)
{
    if (m_pscClient)
        m_pscClient->Destroy();
}

void CTcpIpClientInterface::OpenPipe()
{
    CBlockDataInterface::OpenPipe();
    wxIPV4address addr;
    addr.Hostname(m_ip.m_sAddress);
    addr.Service(m_ip.m_nPort);
    if(!m_pscClient) m_pscClient=new wxSocketClient();
    m_pscClient->Connect(addr,false);
    if (!m_pscClient->WaitOnConnect(5))
        throw std::string(m_ip.GetExtendedType()+" WaitOnConnect timed out");
    if (!m_pscClient->IsConnected())
        throw std::string(m_ip.GetExtendedType()+" was refused connection by the server");
}

bool CTcpIpClientInterface::PipeClosed()
{
   if(m_pscClient==0 || !m_pscClient->IsConnected()) return true;
   else if (m_ip.m_bInput) return false;//monitoring thread can detect disconnects
   //Aadditonal check is needed for sessions that don't monitor input buffer
   //because IsConnected is not always detecting disconnects
   // Note: IsData is true if there is data to read of if the other party dropped connection
   else if (!m_pscClient->IsData()) return false;//nothing to read means no disconnects
   else
   {  //simulate a read to detect disconnected session
      const int MAX_READ_BUFFER=10;
      char sz[MAX_READ_BUFFER];
      m_pscClient->Read(sz,MAX_READ_BUFFER-1);
      CheckErrors();
   }
}

void CTcpIpClientInterface::Send(std::string sOut,std::string sInterfaceType)
{
    if(m_pscClient->WaitForWrite(1))//wait to write data
    {
        m_pscClient->Write(sOut.c_str(),(unsigned int)sOut.size());
        CheckErrors();
        if (m_pscClient->LastCount()!=sOut.length())
            throw std::string("CTcpIpServerInterface::Send wrong number of bites written");
    }
    else throw std::string("CTcpIpClientInterface::Send timed out");
    if (m_ip.m_bOption2) ClosePipe();
}

void CTcpIpClientInterface::ClosePipe()
{
    if (m_pscClient)
        m_pscClient->Destroy();
    m_pscClient=0;
}

std::pair<std::string,bool> CTcpIpClientInterface::GetString()
{
    if(!BufferEmpty()) return std::pair<std::string,bool>(GetStringFromBuffer(),true);
    if (!m_pscClient->IsData())//check if there is a new batch of data to read 
       // Note: IsData also becomes true if the other party dropped connection
       return std::pair<std::string,bool>("",false);
    const int MAX_READ_BUFFER=1000;
    char sz[MAX_READ_BUFFER];
    m_pscClient->Read(sz,MAX_READ_BUFFER-1);
    CheckErrors();
    sz[m_pscClient->LastCount()]=0;
    AddData(sz);
    if(!BufferEmpty()) return std::pair<std::string,bool>(GetStringFromBuffer(),true);
    return std::pair<std::string,bool>("",false);
}

void CTcpIpClientInterface::CheckErrors()
{
    if(m_pscClient->Error())
    {
        std::ostringstream oss;
        oss<<m_pscClient->LastError();;
        m_pscClient->Destroy();
        m_pscClient=0;
        throw std::string("CTcpIpClientInterface read/write error: "+oss.str());
    }
}
