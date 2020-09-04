///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

#include "BlockDataInterface.h"

CBlockDataInterface::CBlockDataInterface(CInterfaceCallBack* picb,
                                 CInterfaceProperties ip): CInterface(picb,ip)
{
}

std::string CBlockDataInterface::GetStringFromBuffer()
{
   if (BufferEmpty())
      throw std::string("CBlockDataInterface::GetStringFromBuffer trying to read an empty buffer");
   std::string s;
   if(m_ip.m_nSplitLines)
   {
      s=m_sInBuffer.substr(0,m_ip.m_nSplitLines);
      m_sInBuffer=m_sInBuffer.substr(m_ip.m_nSplitLines);
   }
   else
   {
      s=m_sInBuffer.substr(0,m_sInBuffer.find(m_ip.m_sEolIn));
      m_sInBuffer=m_sInBuffer.substr(m_sInBuffer.find(m_ip.m_sEolIn)+m_ip.m_sEolIn.size());
   }
   return s;
}

bool CBlockDataInterface::BufferEmpty()
{
   if (m_sInBuffer.empty()) return true;
   if(m_ip.m_nSplitLines)
   {
      if(m_sInBuffer.size()<m_ip.m_nSplitLines)
         return true;
   }
   else if (m_sInBuffer.find(m_ip.m_sEolIn)==std::string::npos)
   {
      if(m_sInBuffer.size()>10000)
         throw std::string("CBlockDataInterface::BufferEmpty linefeeds check failed");
      return true;
   };
   return false;
}

void CBlockDataInterface::OpenPipe()
{
    m_sInBuffer="";
}

void CBlockDataInterface::AddData(std::string sIn)
{
   m_sInBuffer+=sIn;
}
