///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

//Class declaration
#include "UserInterface.h"
//STL
#include <ctime>
#include <iostream>
#include <fstream>

//wxWidgets
#include <wx/datetime.h>

CUserInterface::CUserInterface() : m_bStatusLog(false){}

std::string CUserInterface::GetStatusLogName()
{
	if (!m_bStatusLog)//Update log file
		throw std::string("CUserInterface::GetStatusLogName called before SetStatusLogName");
	//get date
   char sz[100];
   std::time_t tCurrent=std::time(0);
   std::strftime(sz,100,"%j%Y",std::localtime(&tCurrent));
	return m_sStatusLog+"-"+sz+".log";
}

std::string CUserInterface::GetDateTime()
{
   wxDateTime now = wxDateTime::UNow();
   return std::string(now.Format("%m/%d/%y %H:%M:%S:%l ", wxDateTime::Local).c_str());
}

void CUserInterface::UpdateStatus(std::string s)
{
   try
   {
	   wxMutexLocker singleLock(m_CritSection);//.usage of shared resource..
	   if (!singleLock.IsOk())// resource locked
         throw std::string("Error: CUserInterface::UpdateStatus timed out on Critical Section");
      if(m_bStatusLog)
      {
         std::ofstream ofIntegration(GetStatusLogName().c_str(),std::ios::ate|std::ios::app);
         if(!ofIntegration)
            throw std::string("Error: CUserInterface::UpdateStatus can't write to log file");
         ofIntegration<<GetDateTime()<<s<<std::endl;
	      ofIntegration.close();         
      }
      std::cerr<<s<<std::endl;
   }
   catch(std::string sError)
   {
      std::cerr<<s<<std::endl;
   }
}

void CUserInterface::SetStatusLogName(std::string sStatus)
{
   m_sStatusLog=sStatus;
   m_bStatusLog=true;
}

void CUserInterface::LogError(std::string s)
{
   s="Error: "+s;
   UpdateStatus(s);
}

std::string CUserInterface::GetCharCodes(std::string s)
{
   std::string sOut="{";
   for(int i=0;i<s.length();i++)
   {
      char sz[10];
      itoa(*(s.c_str()+i),sz,10);
      if(i!=0) sOut+=",";
      sOut+=sz;
   };
   return sOut+"}";
}