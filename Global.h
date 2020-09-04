///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

#ifndef GLOBAL_H
#define GLOBAL_H

//STL
#include <string>
#include <ctime>

//Local dependancies
#include "UserInterface.h"

extern CUserInterface g_ui;

extern volatile bool g_bQuit;
extern volatile bool g_bQuiet;
extern volatile bool g_bVerbose;
extern std::string g_sName;
extern long g_nBufSize;
extern bool CheckIfItsTime(std::time_t& t, int nDatFreq, std::string sDataType);
extern std::string GetDateTime();
extern std::string strchrn(std::string str, int n, char c);

#endif //GLOBAL_H