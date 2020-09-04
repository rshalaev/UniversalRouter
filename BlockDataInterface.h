///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

#ifndef BLOCKDATA_H
#define BLOCKDATA_H

//Local dependancies
#include "Interface.h"

class CBlockDataInterface: public CInterface
{
public:
    CBlockDataInterface(CInterfaceCallBack* picb,CInterfaceProperties ip);
protected:
    std::string     GetStringFromBuffer();
    bool            BufferEmpty();
    virtual void    OpenPipe();
    void            AddData(std::string sIn);
private:
    std::string     m_sInBuffer;
};

#endif//BLOCKDATA_H
