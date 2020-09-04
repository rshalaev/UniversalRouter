///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

#ifndef SERIALPORTINTERFACE_H
#define SERIALPORTINTERFACE_H

//wxWidgets
#include "wx\ctb\serport.h"

//Local dependencies
#include "BlockDataInterface.h"

class CSerialPortInterface : public CBlockDataInterface
{
public:
    CSerialPortInterface(CInterfaceCallBack* picb,CInterfaceProperties ip);
    virtual ~CSerialPortInterface(void);
protected:
    virtual std::pair<std::string,bool> GetString();
    virtual void OpenPipe();
    virtual bool PipeClosed();
    virtual void Send(std::string sOut,std::string sInterfaceType);
private:
    wxSerialPort    m_wxsp;//Serial port
};

#endif//SERIALPORTINTERFACE_H
