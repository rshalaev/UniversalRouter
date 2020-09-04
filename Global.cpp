///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

//Local dependencies
#include "Global.h"

//STL
#include <sstream>

//wxWidgets
#include <wx/datetime.h>

volatile bool g_bQuit=false;
volatile bool g_bQuiet=false;
volatile bool g_bVerbose=false;
std::string g_sName="RC";
long g_nBufSize=100;

CUserInterface g_ui;

extern bool CheckIfItsTime(std::time_t& t, int nDatFreq, std::string sDataType)
{//return true if it's time to send data and update t variable with current time, nDatFreq is period in seconds
    std::time_t tCurr=std::time(NULL);//UTC time
    //check if it's time to send static data
    if ((tCurr-t) < (nDatFreq)) return false;
    else
    {
       t=tCurr;
       return true;
    }
}



extern std::string GetDateTime()
{
   wxDateTime now = wxDateTime::UNow();
   return std::string(now.Format("%m/%d/%y %H:%M:%S:%l ", wxDateTime::Local).c_str());
};

// find n'th repetition of char c in string str ___ returns index of c in str + 1
extern std::string strchrn(std::string str, int n, char c)
{
    std::string::size_type index=-1;
    for(int i=0;i<n;i++)
    {
        index=str.find(c,index+1);
        if(index==std::string::npos)
        {
            std::stringstream ss;
            std::string s;
            ss << c;
            ss >> s;
            throw std::string("Function strchrn can't find "+s+" in " +str);
        }
    }
    return str.substr(++index);
}
