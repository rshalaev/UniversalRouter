///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

//Local dependencies
#include "FileInterface.h"

//wxWidgets
#include <wx/stopwatch.h>

CFileInterface::CFileInterface(CInterfaceCallBack* picb, CInterfaceProperties ip)
    :CInterface(picb,ip),m_bTimeDifSet(false),m_lTimeDif(0),m_lTimeLast(0)
{
}

CFileInterface::~CFileInterface()
{
    if(!PipeClosed()) m_fLog.close();
}

wxLongLong CFileInterface::ParseUTime(std::string s)
{
//    std::strftime(sz,100,"%m/%d/%y %H:%M:%S:%l ",std::localtime(&tCurrent));
    if(!(s.at(2)=='/' && s.at(5)=='/' && s.at(8)==' ' && s.at(11)==':' && s.at(14)==':' && s.at(17)==':' && s.at(21)==' '))
       throw std::string("Invalid time tag in line "+s);
    time_t t;
    struct tm* timeinfo;
    time ( &t);
    timeinfo = localtime ( &t);
    timeinfo->tm_year = atoi(strchrn(s,2,'/').c_str())- 1900;
    if(timeinfo->tm_year<0)timeinfo->tm_year+=2000;
    timeinfo->tm_mon = atoi(s.c_str()) - 1;
    timeinfo->tm_mday = atoi(strchrn(s,1,'/').c_str());
    timeinfo->tm_hour = atoi(strchrn(s,1,' ').c_str());
    timeinfo->tm_min = atoi(strchrn(s,1,':').c_str());
    timeinfo->tm_sec = atoi(strchrn(s,2,':').c_str());
    t=std::mktime(timeinfo);
    wxLongLong ll=t*1000;
    //Check if milliseconds are present before the second space
    if (strchrn(s,3,':').length()>strchrn(s,2,' ').length())
       ll+=atoi(strchrn(s,3,':').c_str());
    return ll;
}

std::pair<std::string,bool> CFileInterface::GetString()
{
   std::string s;
   if(!std::getline(m_fLog,s))
      throw std::string("CFileInterface::GetString can't read from file");
   s+="\n";
   if(s.find(m_ip.m_sEolIn)==std::string::npos)
      throw std::string("CFileInterface::GetString read a string with missing EOL : [" + s +"] : "+CUserInterface::GetCharCodes(s));
   if(!s.substr(s.find(m_ip.m_sEolIn)+m_ip.m_sEolIn.size()).empty())
      g_ui.LogError("CFileInterface::GetString read a string with characters after EOL : [" + s +"] : "+CUserInterface::GetCharCodes(s));
   s=s.substr(0,s.find(m_ip.m_sEolIn));
   wxLongLong lStringTime;
   //Read strings at time interval specified via command line
   if(m_ip.m_bOption1)
   {   
	   if(m_lTimeLast==0) //first time sending a string
          //time in nanoseconds
		   m_lTimeLast = wxGetLocalTimeMillis()*1000-m_ip.m_nPort;
	   lStringTime = m_lTimeLast+m_ip.m_nPort;
	   m_lTimeLast+=m_ip.m_nPort;
   }
   //Read strings at time interval specified in text file
   else 
   {
      lStringTime=ParseUTime(s)*1000;
      if(!m_bTimeDifSet)
      {
         m_bTimeDifSet=true;
         m_lTimeDif=wxGetLocalTimeMillis()*1000-lStringTime;
      }
   }
   //wait till it's time to send out string    //time in nanoseconds
   while ((wxGetLocalTimeMillis()*1000)<(m_lTimeDif+lStringTime) && !m_bQuit)
   {
      wxLongLong nSleep=(m_lTimeDif+lStringTime)/1000-wxGetLocalTimeMillis();
      if (nSleep>1000) wxMilliSleep(1000);
	  else if (nSleep>0) wxMilliSleep(nSleep.ToLong());
   };
   if (!m_ip.m_bOption2)//Remove tags
   {
      //Remove time tag
      if(s.at(2)=='/' && s.at(5)=='/' && s.at(8)==' ' && s.at(11)==':' && s.at(14)==':' && s.at(17)==':' && s.at(21)==' ')
         s=s.substr(22);
      //Remove interface tag
      size_t pos1=s.find(':');//find first occurance of :
      if(pos1!=std::string::npos)
      {
         size_t pos2=s.find(':',pos1+1);//find second occurance of :
         if(pos2!=std::string::npos)
         {
            size_t pos3=s.find(' ',pos2+1);//find space after 2 occurance of :
            if (pos3!=std::string::npos && s.find(' ')==pos3)
               s=s.substr(pos3+1);
         }
      }
   };
   return std::pair<std::string,bool>(s,true);
}

void CFileInterface::OpenPipe()
{
   m_fLog.open(m_ip.m_sAddress.c_str(), std::fstream::in|std::fstream::binary);
   m_fLog.seekg(std::ios_base::beg);
}

bool CFileInterface::PipeClosed()
{
    if(!m_fLog.is_open()) return true;
    if(m_fLog.eof())
    {
       if(m_ip.m_bReopenClosedPipe)
       {
          m_bTimeDifSet=false;
          m_fLog.clear();
          m_fLog.seekg(std::ios_base::beg);
          if (!m_fLog.eof()) return false;
       }
       m_fLog.close();
       return true;
    }
    return false;
}
