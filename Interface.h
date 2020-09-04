///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

#ifndef INTERFACE_H
#define INTERFACE_H

//STL
#include <string>
#include <vector>

//wxWidgets
#include "wx/thread.h"

//Local defenitions
#include "Global.h"
#include "InterfaceProperties.h"
#include "StrBuffer.h"

class CInterfaceCallBack //prefix icb
{
public:
    virtual void ReceiveString(std::string s, CInterfaceProperties ip)=0;
    virtual void AddTcpIpServerSessionInterface(/*CTcpIpServerSessionInterface*/void* p_ife)=0;
};

class CInterface;

class CInterfaceThread:private wxThread //prefix it
{
public:
   CInterface* m_pife;
   volatile bool m_bThreadRunning;
   CInterfaceThread(CInterface* ife):wxThread(wxTHREAD_JOINABLE),m_pife(ife),m_bThreadRunning(false)
      ,m_bRead(false),m_bWrite(false),m_bCmd(false){;};
   virtual void *Entry();
   bool m_bRead;
   bool m_bWrite;
   bool m_bCmd;
   void Start();
};

class CInterface//prefix ifez
{
public:
    CInterface(CInterfaceCallBack* picb, CInterfaceProperties ip);
    virtual ~CInterface(){;};
    CBuffer<std::string> m_sbufCmd;
    CBuffer<std::pair<std::string,CInterfaceProperties>> m_sbuf;
    CInterfaceProperties m_ip;
    void Init();
    CInterfaceCallBack* m_picb;
    volatile bool m_bQuit;
    bool ThreadsRunning(){return (m_it.m_bThreadRunning || m_itOut.m_bThreadRunning);}
protected:
    virtual std::pair<std::string,bool> GetString()=0;
    virtual void OpenPipe()=0;
    virtual bool PipeClosed()=0;
    virtual void Send(std::string sOut,std::string sInterfaceType)=0;
    virtual void ClosePipe(){;};
private:
    void CheckPipe();
    wxMutex    m_CritSection;
    CInterfaceThread m_it;//Input or Input/output for duplex
    CInterfaceThread m_itOut;//Output for non/duplex
    friend class CInterfaceThread;
    volatile bool m_bPipeInit;
};

#endif//INTERFACE_H
