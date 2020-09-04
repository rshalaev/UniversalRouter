///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

#ifndef TCPIPCLIENTINTERFACE_H
#define TCPIPCLIENTINTERFACE_H

//wxWidgets
#include "wx/socket.h"

//Local dependencies
#include "BlockDataInterface.h"

class CTcpIpClientInterface : public CBlockDataInterface
{
public:
    CTcpIpClientInterface(CInterfaceCallBack* picb,CInterfaceProperties ip);
    virtual ~CTcpIpClientInterface(void);
protected:
    virtual std::pair<std::string,bool> GetString();
    virtual void OpenPipe();
    virtual bool PipeClosed();
    virtual void Send(std::string sOut,std::string sInterfaceType);
    virtual void ClosePipe();
private:
    wxSocketClient* m_pscClient;
    void    CheckErrors();
};

#endif//TCPIPCLIENTINTERFACE_H
