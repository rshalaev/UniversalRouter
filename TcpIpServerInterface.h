///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

#ifndef TCPIPSERVERINTERFACE_H
#define TCPIPSERVERINTERFACE_H

//wxWidgets
#include "wx/socket.h"

//Local dependencies
#include "BlockDataInterface.h"

class CTcpIpServerInterface : public CInterface
{
public:
    CTcpIpServerInterface(CInterfaceCallBack* picb,CInterfaceProperties ip);
    virtual ~CTcpIpServerInterface(void);
protected:
    virtual std::pair<std::string,bool> GetString();
    virtual void OpenPipe();
    virtual bool PipeClosed();
    virtual void Send(std::string sOut,std::string sInterfaceType);
private:
    void CheckErrors();
    wxSocketServer* m_pssServer;
};

class CTcpIpServerSessionInterface : public CBlockDataInterface
{
public:
   CTcpIpServerSessionInterface(CInterfaceCallBack* picb,CInterfaceProperties ip,wxSocketBase* psbConnection, std::vector<std::pair<std::string,CInterfaceProperties>> v);
    virtual ~CTcpIpServerSessionInterface(void);
protected:
    virtual std::pair<std::string,bool> GetString();
    virtual void OpenPipe();
    virtual bool PipeClosed();
    virtual void Send(std::string sOut,std::string sInterfaceType);
private:
    void CheckErrors();
    wxSocketBase*   m_psbConnection;
};

#endif//TCPIPSERVERINTERFACE_H
