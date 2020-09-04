///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

#ifndef STDINTERFACE_H
#define STDINTERFACE_H

//Local dependencies
#include "StrBuffer.h"
#include "Interface.h"

class CStdInterface : public CInterface //prefix sife
{
//StdIn port watch thread logs incoming characters to the message queue that other threads can poll
public:
    CStdInterface(CInterfaceCallBack* picb,CInterfaceProperties ip);
    virtual std::pair<std::string,bool> GetString();
protected:
    virtual void OpenPipe();
    virtual bool PipeClosed();
    virtual void Send(std::string sOut, std::string sInterfaceType);
};

#endif//STDINTERFACE_H
