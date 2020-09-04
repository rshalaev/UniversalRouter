///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

//Local dependencies
#include "Interface.h"

//STL
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <exception>

//wxWidgets
#include "wx/utils.h"

CInterface::CInterface(CInterfaceCallBack* picb, CInterfaceProperties ip):
            m_ip(ip), m_picb(picb),
            m_sbuf(g_nBufSize,0,"InterfaceOutputBuffer",m_ip.m_bExp,m_ip.m_nExpTime),m_sbufCmd(100,100,"CommandBuffer",false,-1)
            ,m_bQuit(false),m_it(this),m_itOut(this),m_bPipeInit(false)
{
};


void CInterfaceThread::Start()
{
   if (Create() != wxTHREAD_NO_ERROR)//Start stdin monitor
      throw std::string("Can't create monitor thread!");
   Run();
}

void CInterface::Init()
{
    m_ip.Print();
    if(m_ip.m_bInput )
    {
       m_it.m_bCmd=true;
       m_it.m_bRead=true;
       if (!m_ip.m_bDuplex && m_ip.m_bOutput) m_it.m_bWrite=true;
       m_it.Start();
       g_ui.UpdateStatus("Started monitor thread for interface: "+m_ip.GetExtendedType());
    }
    if(m_ip.m_bOutput && (m_ip.m_bDuplex || !m_ip.m_bInput))
    {
       if(!m_ip.m_bInput)m_itOut.m_bCmd=true;
       m_itOut.m_bRead=false;
       m_itOut.m_bWrite=true;
       m_itOut.Start();
       g_ui.UpdateStatus("Started output thread for interface: "+m_ip.GetExtendedType());
    }
}

void CInterface::CheckPipe()
{
    if (PipeClosed())
    {
        if(!m_ip.m_bReopenClosedPipe)
        {
            m_bQuit=true;
            throw std::string("CInterface::CheckPipe found closed pipe");
        }
        else
        {   //Reopen pipe
        	   wxMutexLocker singleLock(m_CritSection);
            if (!singleLock.IsOk())// lock resource 
               throw std::string("CInterface::CheckPipe timed out on Critical Section");
            OpenPipe();
            if (PipeClosed())
               throw std::string("CInterface::CheckPipe failed to open a closed pipe for interface: "+m_ip.GetType());
        }
    }
}

void* CInterfaceThread::Entry()
{//worker thread
   m_bThreadRunning=true;
   if(!m_pife->m_bPipeInit)
   {//first thread to start needs to open pipe
      m_pife->m_bPipeInit=true;
      wxMutexLocker singleLock(m_pife->m_CritSection);
      if (!singleLock.IsOk())// lock resource 
         throw std::string("CInterface::CheckPipe timed out on Critical Section");
      try
      {
         m_pife->OpenPipe();
      }
      catch(std::string sError)
      {
         if(!m_pife->m_ip.m_bNoErrors && 
            sError!="CTcpIpServerInterface::OpenPipe can't establish connection" &&
            sError!="CInterface::CheckPipe failed to open a closed pipe for interface: TcpIpServer" &&
            !(sError=="CInterface::CheckPipe found closed pipe" && m_pife->m_ip.m_it==TcpIpServerSession))
            g_ui.LogError("CInterface::Entry caught exception: "+sError);
      }
   }
   int nSleepTime=0;
   while (!m_pife->m_bQuit && !g_bQuit)
   {
      try
      {
         //Execute commands
         if(m_bCmd && !m_pife->m_sbufCmd.IsEmpty() && !m_pife->m_bQuit && !g_bQuit)
         {
            std::string s=m_pife->m_sbufCmd.Get();
            if(s=="close pipe") m_pife->ClosePipe();
            else if(s=="quit") m_pife->m_bQuit=true;
            else throw std::string("unknown command: "+s);
         }
         if (m_pife->m_ip.m_it==TcpIpServer)
         {//Handle TCP/IP Server as special case
            m_pife->OpenPipe();
            continue;
         }
         //Read Data
         if (m_bRead && !m_pife->m_bQuit && !g_bQuit)
         {
            m_pife->CheckPipe();
            std::pair<std::string,bool> prSin=m_pife->GetString();
            if (prSin.second)
            {
               std::string s=prSin.first;
               nSleepTime=0;
               //Execute Quit Command
               if (s==(g_sName+" quit"))
               {
                  g_bQuit=true;
                  m_pife->m_bQuit=true;
               }
               //Broadcast Command
               else m_pife->m_picb->ReceiveString(s,m_pife->m_ip);
            }
         }
         //Send Data
         if (m_bWrite && !m_pife->m_sbuf.m_bEmpty && !m_pife->m_bQuit && !g_bQuit)
         {
            nSleepTime=0;
            m_pife->CheckPipe();
            std::pair<std::string ,CInterfaceProperties> pr=m_pife->m_sbuf.Get();
            std::string sOut=pr.first+m_pife->m_ip.m_sEolOut;
            if(g_bVerbose) g_ui.UpdateStatus(m_pife->m_ip.GetExtendedType()+
               " sending string from "+pr.second.GetExtendedType() + " : [" + 
               pr.first + "] : " + CUserInterface::GetCharCodes(sOut));
            m_pife->Send(sOut,pr.second.GetExtendedType());
         }
         if (nSleepTime!=0)
         {//Sleep when no data is available
            if(nSleepTime>50)
            {
               nSleepTime=50;
               //Check pipe state in case there was no data read or write
               m_pife->CheckPipe();
            }
            wxMilliSleep(nSleepTime);
         }
         nSleepTime++;
      }
      catch(std::string sError)
      {
         if(!m_pife->m_ip.m_bNoErrors &&
            !(sError=="CInterface::CheckPipe found closed pipe" && m_pife->m_ip.m_it==TcpIpServerSession))
            g_ui.LogError("CInterface::Entry caught exception: "+sError);
         wxSleep(1);
      }
   }
   g_ui.UpdateStatus("Quit worker thread for interface:"+m_pife->m_ip.GetExtendedType());
   m_bThreadRunning=false;
   return 0;
}
