///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

//Local dependencies
#include "StdInterface.h"

//STL
#include <iostream>

CStdInterface::CStdInterface(CInterfaceCallBack* picb, CInterfaceProperties ip)
    :CInterface(picb,ip)
{
}

std::pair<std::string,bool> CStdInterface::GetString()
{
   std::string s;
   getline(std::cin,s);
   s+="\n";
   if(s.find(m_ip.m_sEolIn)==std::string::npos)
      throw std::string("CStdInterface::GetString read a string with missing EOL : [" + s +"] : "+CUserInterface::GetCharCodes(s));
   if(!s.substr(s.find(m_ip.m_sEolIn)+m_ip.m_sEolIn.size()).empty())
      g_ui.LogError("CStdInterface::GetString read a string with characters after EOL : [" + s +"] : "+CUserInterface::GetCharCodes(s));
   s=s.substr(0,s.find(m_ip.m_sEolIn));
   return std::pair<std::string,bool>(s,true);
}

void CStdInterface::OpenPipe()
{
}

bool CStdInterface::PipeClosed()
{
    return std::cin.eof();
}

void CStdInterface::Send(std::string sOut, std::string sInterfaceType)
{
   if(m_ip.m_nPort==0 || m_ip.m_nPort==2)
   {
      std::cout<<sOut;
      std::flush(std::cout);
   }
   if(m_ip.m_nPort==1 || m_ip.m_nPort==2)
      g_ui.UpdateStatus(sOut.substr(0,sOut.find_first_of(m_ip.m_sEolOut)));
}
