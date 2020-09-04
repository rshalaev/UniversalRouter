///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

//STL
#include <string>
//wxWidgets
#include "wx\thread.h"

class CUserInterface
{
public:
   CUserInterface();
   void UpdateStatus(std::string s);
   void LogError(std::string s);
	void SetStatusLogName(std::string sStatus);
   static std::string GetCharCodes(std::string s);
private:
	std::string m_sStatusLog;
   bool        m_bStatusLog;
   std::string GetDateTime();
	std::string GetStatusLogName();
   wxMutex     m_CritSection;
};

#endif //USERINTERFACE_H