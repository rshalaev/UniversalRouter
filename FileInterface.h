///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

#ifndef FILEINTERFACE_H
#define FILEINTERFACE_H

//STL
#include <fstream>

//Local dependencies
#include "StrBuffer.h"
#include "Interface.h"

//wxWidgets
#include <wx/longlong.h>

class CFileInterface:public CInterface//prefix fi
{
public:
    CFileInterface(CInterfaceCallBack* picb,CInterfaceProperties ip);
	 virtual ~CFileInterface();
    virtual std::pair<std::string,bool> GetString();
protected:
    virtual void OpenPipe();
    virtual bool PipeClosed();
    virtual void Send(std::string sOut,std::string sInterfaceType){;};
private:
    wxLongLong ParseUTime(std::string s);
    std::fstream m_fLog;
    wxLongLong m_lTimeDif;//difference between start time of a file and program start timeh
	wxLongLong m_lTimeLast;//time of the last sent out string
    bool m_bTimeDifSet;
};

#endif//FILEINTERFACE_H
