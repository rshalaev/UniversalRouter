///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

#ifndef MULTIPLEXOR_H
#define MULTIPLEXOR_H

//Local dependencies
#include "Interface.h"
//STL
#include <memory>

class CMultiplexor: public CInterfaceCallBack //prefix mpx
{
public:
   CMultiplexor();
   virtual ~CMultiplexor();
   void Broadcast();
   virtual void ReceiveString(std::string s, CInterfaceProperties ip);
   void AddInterface(CInterfaceProperties ip);
   virtual void AddTcpIpServerSessionInterface(/*CTcpIpServerSessionInterface*/void* p_ife);
   void DeleteDisconnectedInterfaces();
   static std::vector<CInterfaceProperties> ParseCommandLine(int argc, char* argv[]);
private:
   CBuffer<std::pair<std::string,CInterfaceProperties>>        m_sbuf;
   std::vector<CInterface*>    m_vpife;
   int m_nSleep;
   wxMutex        m_CritSection;
   static std::string GetEol(bool bUnixEol,bool bEol,std::string sEol);
   static std::vector<std::string> CMultiplexor::GetVector(std::string sIn,char cDelim);
};

#endif//MULTIPLEXOR_H
