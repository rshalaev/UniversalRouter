///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

#ifndef TESTINTERFACE_H
#define TESTINTERFACE_H

//STL
#include <fstream>

//Local dependencies
#include "StrBuffer.h"
#include "Interface.h"

//wxWidgets
#include <wx/longlong.h>

class CTestInterface:public CInterface//prefix ti
{
public:
    CTestInterface(CInterfaceCallBack* picb,CInterfaceProperties ip);
    virtual std::pair<std::string,bool> GetString();
protected:
   virtual void OpenPipe(){;};
   virtual bool PipeClosed(){return false;};
   virtual void Send(std::string sOut,std::string sInterfaceType){;};
private:
   wxLongLong m_lTimeDif;//difference between start time of a file and program start timeh
   bool m_bTimeDifSet;
   long long m_llLine;
};

#endif//TESTINTERFACE_H
