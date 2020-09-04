///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

//Local dependancies
#include "InterfaceProperties.h"
#include "Global.h"

//STL
#include <iostream>

#define stringify( name ) # name

//reference by InterfaceTypeNames[it]
const char* InterfaceTypeNames[] = 
{
  stringify( Serial ),
  stringify( TcpIpClient ),
  stringify( TcpIpServer ),
  stringify( StdIO ),
  stringify( Log ),
  stringify( TcpIpServerSession ),
  stringify( File ),
  stringify( Test )
};

bool CInterfaceProperties::Equal(CInterfaceProperties& ip)
{
   //Don't send data back to the interface that received the data
   if(ip.m_it==m_it && ip.m_nPort==m_nPort && ip.m_sAddress==m_sAddress) return true;
   //Don't share data between server sessions if option 2 is not set to share (true)
   else if(m_it==TcpIpServerSession  && ip.m_it==TcpIpServerSession && !m_bOption2 && !ip.m_bOption2 && m_nPeerPort==ip.m_nPeerPort)
      return true;
   //Share data
   else return false;
}

bool CInterfaceProperties::Match(CInterfaceProperties& ip)
{
   //if to and from types match return true
   if(m_sTo.empty() && ip.m_sFrom.empty()) return true;
   for(int i=0;i<m_sTo.size();i++)
      if(ip.m_sFrom.find(m_sTo.c_str()+i)!=std::string::npos) return true;
   return false;
}

CInterfaceProperties::CInterfaceProperties(void):m_bNoErrors(false),m_nPort(-1), m_nPeerPort(-1),
   m_nExpTime(0), m_bExp(false),m_bEcho(false), m_bSuspendOutput(false), m_nSplitLines(0),m_bOption1(false),m_bOption2(false),
   m_bOption3(false)
{
}

CInterfaceProperties::~CInterfaceProperties(void)
{
}

std::string itos(int i)
{
    char sz[100];
    itoa(i,sz,10);
    return std::string(sz);
}

std::string CInterfaceProperties::GetExtendedType()
{
   if(m_it==InterfaceType::TcpIpServerSession)
      return std::string(InterfaceTypeNames[m_it])+":"+m_sPeerAddress+":"+itos(m_nPeerPort)+" p:"+itos(m_nPort);
   else
	   return std::string(InterfaceTypeNames[m_it])+":"+m_sAddress+":"+itos(m_nPort);
}
std::string CInterfaceProperties::GetType()
{
	return std::string(InterfaceTypeNames[m_it]);
}

void CInterfaceProperties::Print()
{
    g_ui.UpdateStatus("Interface: "+GetType()+" Input:"+itos(m_bInput)+" Output:"+itos(m_bOutput)
       +" Address:"+m_sAddress.c_str()+" Port:"+itos(m_nPort));
}
