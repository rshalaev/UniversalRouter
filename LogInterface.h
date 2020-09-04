///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

#ifndef LOGINTERFACE_H
#define LOGINTERFACE_H

//STL
#include <fstream>

//Local dependencies
#include "StrBuffer.h"
#include "Interface.h"

//wxWidgets
#include <wx/longlong.h>

class CLogInterface : public CInterface //prefix sife
{
//Log file watch thread logs time-stamped strings in to a log file
public:
    CLogInterface(CInterfaceCallBack* picb,CInterfaceProperties ip);
	 virtual ~CLogInterface();
    virtual std::pair<std::string,bool> GetString(){throw std::string("CLogInterface GetString called");};
protected:
    virtual void OpenPipe();
    virtual bool PipeClosed();
    virtual void Send(std::string sOut,std::string sInterfaceType);
private:
    std::fstream m_fLog;
    std::string  m_sLog;
    std::string GetFileName();
    std::time_t m_tLast;//last time data was saved to file
};

#endif//LOGINTERFACE_H
