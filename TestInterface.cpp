///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

//Local dependencies
#include "TestInterface.h"
#include "Global.h"

//wxWidgets
#include <wx/stopwatch.h>

CTestInterface::CTestInterface(CInterfaceCallBack* picb, CInterfaceProperties ip)
    :CInterface(picb,ip),m_bTimeDifSet(false),m_lTimeDif(0),m_llLine(0)
{
}

std::pair<std::string,bool> CTestInterface::GetString()
{
   std::string s;
   m_lTimeDif+=m_ip.m_nPort;
   if(!m_bTimeDifSet)
   {
      m_bTimeDifSet=true;
      m_lTimeDif=wxGetLocalTimeMillis()-m_ip.m_nPort;
   }
   //pause
   while (wxGetLocalTimeMillis()<m_lTimeDif+m_ip.m_nPort && !m_bQuit)
   {
      wxLongLong nSleep=(m_lTimeDif+m_ip.m_nPort)-wxGetLocalTimeMillis();
      if (nSleep>1000) nSleep=1000;
      wxMilliSleep(nSleep.ToLong());
   }
   if(m_ip.m_sAddress.empty())
   {
      char sz[20];
      itoa(m_llLine,sz,10);
      s="[ "+GetDateTime()+" "+sz+" ]";
      m_llLine++;
   }
   else s=m_ip.m_sAddress;
   s=strchrn("   "+s,3,' ');
   return std::pair<std::string,bool>(s,true);
}
