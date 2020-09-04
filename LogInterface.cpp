///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

//Local dependencies
#include "LogInterface.h"

//STL
#include <iostream>
#include <ctime>
#include <sstream>

//wxWidgets
#include <wx/utils.h>
#include <wx/timer.h>
#include <wx/datetime.h>
#include <wx/stopwatch.h>

CLogInterface::CLogInterface(CInterfaceCallBack* picb, CInterfaceProperties ip):CInterface(picb,ip), m_tLast(0)
{
}

CLogInterface::~CLogInterface()
{
    if(!PipeClosed()) m_fLog.close();
}

void CLogInterface::OpenPipe()
{
    m_sLog=GetFileName();
    m_fLog.open(m_sLog.c_str(), std::fstream::out | std::fstream::binary | std::fstream::app/*append after the end of file*/);
}

bool CLogInterface::PipeClosed()
{
    if(!m_fLog.is_open()) return true;
    else return false;
}

std::string CLogInterface::GetFileName()
{
   //get date
   char sz[100];
   std::time_t tCurrent=std::time(0);
   std::strftime(sz,100,"%j%Y",std::localtime(&tCurrent));
   return std::string(m_ip.m_sAddress.c_str())+"-"+sz+".log";
}

void CLogInterface::Send(std::string sOut, std::string sInterfaceType)
{
   if((!m_ip.m_bOption3 && m_sLog!=GetFileName()) || (m_ip.m_nPort!=-1 && CheckIfItsTime(m_tLast,m_ip.m_nPort,"")))
   {  //reopen pipe if date changed or if file needs to be periodically saved
      m_fLog.close();//pipe should be already open, so just close it
      OpenPipe();
      if(PipeClosed())throw std::string("CLogInterface::Send string can't reopen pipe");
   }
   if(m_ip.m_bOption1)m_fLog<<GetDateTime();
   if(m_ip.m_bOption2)m_fLog<<sInterfaceType<<" ";
   m_fLog<<sOut;
}
