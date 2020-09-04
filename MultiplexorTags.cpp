///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

//Local dependencies
#include "Multiplexor.h"
#include "StdInterface.h"
#include "TcpIpServerInterface.h"
#include "TcpIpClientInterface.h"
#include "SerialPortInterface.h"
#include "LogInterface.h"
#include "TestInterface.h"
#include "FileInterface.h"

//wxWidgets
#include "wx/utils.h"
#include "wx/cmdline.h"

//STL
#include <iostream>
#include <sstream>
#include <cmath>

std::vector<std::string> CMultiplexor::GetVector(std::string sIn,char cDelim)
{//parse sIn
   for(size_t i=0; i<sIn.size(); i++)
      if (sIn[i]==9) sIn.erase(i,1);
	std::vector<std::string> vs;
	unsigned int j=0,i=0;
	while(i<sIn.length())
	{
		while(j<sIn.length() && (sIn.c_str())[j]!=cDelim) j++;
		vs.push_back(sIn.substr(i,j-i));
      j++;
		i=j;
	}
	return vs;
}

std::string CMultiplexor::GetEol(bool bUnixEol,bool bEol,std::string sEol)
{
   std::string s;
   //Set EOL
   if(bEol)
   {
      std::vector<std::string> vs=GetVector(sEol,',');
      for(int i=0;i<vs.size();i++)
         s+=(char)atoi(vs[i].c_str());
   }
   else if(bUnixEol) s='\n';
   else s="\r\n";
   return s;
}

std::vector<CInterfaceProperties> CMultiplexor::ParseCommandLine(int argc, char* argv[])
{
   //Parse command line
   static const wxCmdLineEntryDesc cmdLineDesc[] =
   {
      {wxCMD_LINE_SWITCH,"h","help","show this help message",wxCMD_LINE_VAL_NONE,wxCMD_LINE_OPTION_HELP},
      {wxCMD_LINE_SWITCH,"?","help","show this help message",wxCMD_LINE_VAL_NONE,wxCMD_LINE_OPTION_HELP},
      {wxCMD_LINE_SWITCH,"v","verbose","be verbose"},
      {wxCMD_LINE_SWITCH,"q","quiet","be quiet"},
      {wxCMD_LINE_SWITCH,"z","unix_eol","use unix eol"},
      {wxCMD_LINE_OPTION,"x","name","name of RC instance",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","buffer","size of i/o buffer",wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","exp_time","#sec to keep strings in buffer",wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},

      {wxCMD_LINE_OPTION,"n","log","log received data in to file",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","logfrom","Log data from interfaces",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","logeol","Log file EOL (CSV char codes)",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_SWITCH,"","logtime","Add timestamp to each log line"},
      {wxCMD_LINE_SWITCH,"","loginterface","Record interface for each log line"},
      {wxCMD_LINE_OPTION,"","logsavefreq","Save log file every x minutes",wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","logcont","Don't start new log file at midnight"},

      {wxCMD_LINE_OPTION,"","log2","log2 received data in to file",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","log2from","Log2 data from interfaces",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","log2eol","Log2 file EOL (CSV char codes)",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_SWITCH,"","log2time","Add timestamp to each Log2 line"},
      {wxCMD_LINE_SWITCH,"","log2interface","Record interface for each Log2 line"},
      {wxCMD_LINE_OPTION,"","log2savefreq","Save Log2 file every x minutes",wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","log2cont","Don't start new Log2 file at midnight"},

      {wxCMD_LINE_OPTION,"","status_log","log program status in to file",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},

      {wxCMD_LINE_OPTION,"","rlog","read data from time stamped file",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_SWITCH,"","rlog_repeat","repeatedly read data from time stamped file",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","rlogto","Send data from file to interfaces",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","rlogeol","Input file EOL (CSV char codes)",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","rlogfreq","Read input file every x microseconds",wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_SWITCH,"","rlognotagskip","Don't skip tags in input file"},

      {wxCMD_LINE_OPTION,"","rep_rate","generate test string every x milliseconds",wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","teststring","test string",wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","teststringto","Send test string data to interfaces",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","teststringeol","Log file EOL (CSV char codes)",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},

      {wxCMD_LINE_SWITCH,"i","stdin","run StdIn monitor"},
      {wxCMD_LINE_SWITCH,"u","stdecho","turn on StdOut/StdErr echo"},
      {wxCMD_LINE_SWITCH,"o","stdout","turn on StdOut output"},
      {wxCMD_LINE_SWITCH,"","stdcerr","turn on StdErr output"},
      {wxCMD_LINE_OPTION,"","stdto","Send StdIO data to interfaces",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","stdfrom","Receive StdIO data from interfaces",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","stdeolin","StdIn EOL (CSV char codes)",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","stdeolout","StdOut EOL (CSV char codes)",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},

      {wxCMD_LINE_SWITCH,"w","tcpsin","run TcpIp server monitor"},
      {wxCMD_LINE_SWITCH,"e","tcpsout","turn on TcpIp server output"},
      {wxCMD_LINE_SWITCH,"s","tcpsecho","turn on TcpIp server echo output"},
      {wxCMD_LINE_SWITCH,"b","tcpsnerr","supress TcpIp server connection errors"},
      {wxCMD_LINE_OPTION,"p","tcpsport","TcpIp server port",wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_SWITCH,"","tcpsbuffer","enable buffer dump for new TcpIp server sessions"},
      {wxCMD_LINE_SWITCH,"","tcpsshare","TcpIp server share data between sessions",wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","tcpsto","Send TcpIp server data to interfaces",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","tcpsfrom","Receive TcpIp server data from interfaces",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","tcpseolin","TcpIp server input EOL (CSV char codes)",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","tcpseolout","TcpIp server output EOL (CSV char codes)",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},

      {wxCMD_LINE_SWITCH,"","tcps2in","run TcpIp2 server monitor"},
      {wxCMD_LINE_SWITCH,"","tcps2out","turn on TcpIp2 server output"},
      {wxCMD_LINE_SWITCH,"","tcps2echo","turn on TcpIp2 server echo output"},
      {wxCMD_LINE_SWITCH,"","tcps2nerr","supress TcpIp2 server connection errors"},
      {wxCMD_LINE_OPTION,"","tcps2port","TcpIp2 server port",wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_SWITCH,"","tcps2buffer","enable buffer dump for new TcpIp2 server sessions"},
      {wxCMD_LINE_SWITCH,"","tcps2share","TcpIp2 server share data between sessions",wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","tcps2to","Send TcpIp2 server data to interfaces",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","tcps2from","Receive TcpIp2 server data from interfaces",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","tcps2eolin","TcpIp2 server input EOL (CSV char codes)",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","tcps2eolout","TcpIp2 server output EOL (CSV char codes)",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},

      {wxCMD_LINE_SWITCH,"r","tcpcin","run TcpIp client monitor"},
      {wxCMD_LINE_SWITCH,"y","tcpcout","turn on TcpIp client output"},
      {wxCMD_LINE_SWITCH,"d","tcpcecho","turn on TcpIp client echo output"},
      {wxCMD_LINE_SWITCH,"c","tcpcnerr","supress TcpIp client connection errors"},
      {wxCMD_LINE_OPTION,"t","tcpcport","TcpIp client port",wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"a","tcpcaddress","TcpIp client address",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_SWITCH,"","tcpcnewsession","TcpIp client disconnect after each line"},
      {wxCMD_LINE_OPTION,"","tcpcto","Send TcpIp client data to interfaces",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","tcpcfrom","Receive TcpIp client data from interfaces",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","tcpceolin","TcpIp client input EOL (CSV char codes)",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","tcpceolout","TcpIp client output EOL (CSV char codes)",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},

      {wxCMD_LINE_SWITCH,"","tcpc2in","run TcpIp2 client monitor"},
      {wxCMD_LINE_SWITCH,"","tcpc2out","turn on TcpIp2 client output"},
      {wxCMD_LINE_SWITCH,"","tcpc2echo","turn on TcpIp2 client echo output"},
      {wxCMD_LINE_SWITCH,"","tcpc2nerr","supress TcpIp2 client connection errors"},
      {wxCMD_LINE_OPTION,"","tcpc2port","TcpIp2 client port",wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","tcpc2address","TcpIp2 client address",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_SWITCH,"","tcpc2newsession","TcpIp2 client disconnect after each line"},
      {wxCMD_LINE_OPTION,"","tcpc2to","Send TcpIp2 client data to interfaces",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","tcpc2from","Receive TcpIp2 client data from interfaces",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","tcpc2eolin","TcpIp2 client input EOL (CSV char codes)",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","tcpc2eolout","TcpIp2 client output EOL (CSV char codes)",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},

      {wxCMD_LINE_SWITCH,"f","serin","run Serial monitor"},
      {wxCMD_LINE_SWITCH,"g","serout","turn on Serial output"},
      {wxCMD_LINE_SWITCH,"j","serecho","turn on Serial echo output"},
      {wxCMD_LINE_OPTION,"k","serbaud","Serial baud",wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"l","sercomm","Serial com port",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","serto","Send serial data to interfaces",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","serfrom","Receive serial data from interfaces",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","sereolin","Serial input EOL (CSV char codes)",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","sereolout","Serial output EOL (CSV char codes)",wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_OPTION,"","sersplit","split in to lines by given # of char",wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_NONE }
   };
   //Parse command line
   wxCmdLineParser parser(cmdLineDesc, argc, argv);
   std::vector<CInterfaceProperties> vip;
   wxString sName;
   switch (parser.Parse() )
   {
      case -1:
          break;//Help printed to stdout
      case 0:
          g_bQuiet=parser.Found("q");
          g_bVerbose=parser.Found("v");
          if (parser.Found("x",&sName)) g_sName=sName.c_str();
          parser.Found("buffer",&g_nBufSize);
          if (parser.Found("status_log",&sName)) g_ui.SetStatusLogName(sName.c_str());
          if (parser.Found("log",&sName))
          {//Log file interface
              CInterfaceProperties ip;
              ip.m_sAddress=sName;
              ip.m_it=Log;
              ip.m_bInput=false;
              ip.m_bOutput=true;
              ip.m_bReopenClosedPipe=true;
              ip.m_bDuplex=false;     
              parser.Found("logeol",&sName);
              ip.m_sEolIn=GetEol(parser.Found("unix_eol"),parser.Found("logeol",&sName),sName.c_str());
              ip.m_sEolOut=GetEol(parser.Found("unix_eol"),parser.Found("logeol",&sName),sName.c_str());
              if(parser.Found("logfrom",&sName))ip.m_sFrom=sName;
              ip.m_bOption1=parser.Found("logtime");
              ip.m_bOption2=parser.Found("loginterface");
              if(parser.Found("logsavefreq",&ip.m_nPort));
              ip.m_bOption3=parser.Found("logcont");
              vip.push_back(ip);
          };
          if (parser.Found("log2",&sName))
          {//Log2 file interface
              CInterfaceProperties ip;
              ip.m_sAddress=sName;
              ip.m_it=Log;
              ip.m_bInput=false;
              ip.m_bOutput=true;
              ip.m_bReopenClosedPipe=true;
              ip.m_bDuplex=false;
              parser.Found("log2eol",&sName);
              ip.m_sEolIn=GetEol(parser.Found("unix_eol"),parser.Found("log2eol",&sName),sName.c_str());
              ip.m_sEolOut=GetEol(parser.Found("unix_eol"),parser.Found("log2eol",&sName),sName.c_str());
              if(parser.Found("log2from",&sName))ip.m_sFrom=sName;
              ip.m_bOption1=parser.Found("log2time");
              ip.m_bOption2=parser.Found("log2interface");
              if(parser.Found("log2savefreq",&ip.m_nPort));
              ip.m_bOption3=parser.Found("log2cont");
              vip.push_back(ip);
          };
          if (parser.Found("rep_rate"))
          {//Test interface
              CInterfaceProperties ip;
              ip.m_it=Test;
              ip.m_bInput=true;
              ip.m_bOutput=false;
              ip.m_bReopenClosedPipe=true;
              ip.m_bDuplex=true;
              parser.Found("teststringeol",&sName);
              ip.m_sEolIn=GetEol(parser.Found("unix_eol"),parser.Found("teststringeol",&sName),sName.c_str());
              ip.m_sEolOut=GetEol(parser.Found("unix_eol"),parser.Found("teststringeol",&sName),sName.c_str());
              if(parser.Found("teststring",&sName))ip.m_sAddress=sName;
              parser.Found("rep_rate",&ip.m_nPort);
              if(parser.Found("teststringto",&sName))ip.m_sTo=sName;
              vip.push_back(ip);
          };
          if (parser.Found("rlog",&sName))
          {//Read log file interface
              CInterfaceProperties ip;
              ip.m_sAddress=sName;
              ip.m_it=File;
              ip.m_bInput=true;
              ip.m_bOutput=false;
              ip.m_bReopenClosedPipe=parser.Found("rlog_repeat");
              ip.m_bDuplex=false;
              parser.Found("rlogeol",&sName);
              ip.m_sEolIn=GetEol(parser.Found("unix_eol"),parser.Found("rlogeol",&sName),sName.c_str());
              ip.m_sEolOut=GetEol(parser.Found("unix_eol"),parser.Found("rlogeol",&sName),sName.c_str());
              if(parser.Found("rlogto",&sName))ip.m_sTo=sName;
              if(parser.Found("rlogfreq",&ip.m_nPort)) ip.m_bOption1=true;
              if(parser.Found("rlognotagskip")) ip.m_bOption2=true;
              vip.push_back(ip);
          };
          if (parser.Found("stdout") || parser.Found("stdcerr") || parser.Found("stdin"))
          {//STDIO interface
              CInterfaceProperties ip;
              ip.m_it=StdIO;
              ip.m_bInput=parser.Found("i");
              ip.m_bEcho=parser.Found("u");
              ip.m_bReopenClosedPipe=false;
              if (parser.Found("stdout")|| parser.Found("stdcerr"))
              {
                  ip.m_bOutput=true;
                  if (parser.Found("stdout") && parser.Found("stdcerr")) ip.m_nPort=2;
                  else if (parser.Found("stdout")) ip.m_nPort=0;
                  else ip.m_nPort=1;
              }
              else ip.m_bOutput=false;
              ip.m_bDuplex=true;
              parser.Found("stdeolin",&sName);
              ip.m_sEolIn=GetEol(true,parser.Found("stdeolin",&sName),sName.c_str());
              parser.Found("stdeolout",&sName);
              ip.m_sEolOut=GetEol(true,parser.Found("stdeolout",&sName),sName.c_str());
              if(parser.Found("stdto",&sName))ip.m_sTo=sName;
              if(parser.Found("stdfrom",&sName))ip.m_sFrom=sName;
              vip.push_back(ip);
          };
          if (parser.Found("w") || parser.Found("e"))
          {//TCP IP server interface
              CInterfaceProperties ip;
              ip.m_it=TcpIpServer;
              ip.m_bInput=parser.Found("w");
              ip.m_bOutput=parser.Found("e");
              ip.m_bEcho=parser.Found("s");
              ip.m_bReopenClosedPipe=true;
              ip.m_bDuplex=false;
              parser.Found("tcpseolin",&sName);
              ip.m_sEolIn=GetEol(parser.Found("unix_eol"),parser.Found("tcpseolin",&sName),sName.c_str());
              parser.Found("tcpseolout",&sName);
              ip.m_sEolOut=GetEol(parser.Found("unix_eol"),parser.Found("tcpseolout",&sName),sName.c_str());
              ip.m_bNoErrors=parser.Found("tcpsnerr");
              ip.m_bOption1=parser.Found("tcpsbuffer");
              ip.m_bOption2=parser.Found("tcpsshare");
              if(!parser.Found("p",&ip.m_nPort))
                  throw std::string("Missing tcp ip server port in command line args");
              if(parser.Found("exp_time",&ip.m_nExpTime))
                  ip.m_bExp=true;
              if(parser.Found("tcpsto",&sName))ip.m_sTo=sName;
              if(parser.Found("tcpsfrom",&sName))ip.m_sFrom=sName;
              vip.push_back(ip);
          };
          if (parser.Found("tcps2in") || parser.Found("tcps2out"))
          {//TCP IP 2 server interface
              CInterfaceProperties ip;
              ip.m_it=TcpIpServer;
              ip.m_bInput=parser.Found("tcps2in");
              ip.m_bOutput=parser.Found("tcps2out");
              ip.m_bEcho=parser.Found("tcps2echo");
              ip.m_bReopenClosedPipe=true;
              ip.m_bDuplex=false;
              parser.Found("tcps2eolin",&sName);
              ip.m_sEolIn=GetEol(parser.Found("unix_eol"),parser.Found("tcps2eolin",&sName),sName.c_str());
              parser.Found("tcps2eolout",&sName);
              ip.m_sEolOut=GetEol(parser.Found("unix_eol"),parser.Found("tcps2eolout",&sName),sName.c_str());
              ip.m_bNoErrors=parser.Found("tcps2nerr");
              ip.m_bOption1=parser.Found("tcps2buffer");
              ip.m_bOption2=parser.Found("tcps2share");
              if(!parser.Found("tcps2port",&ip.m_nPort))
                  throw std::string("Missing tcp ip 2 server port in command line args");
              if(parser.Found("exp_time",&ip.m_nExpTime))
                  ip.m_bExp=true;
              if(parser.Found("tcps2to",&sName))ip.m_sTo=sName;
              if(parser.Found("tcps2from",&sName))ip.m_sFrom=sName;
              vip.push_back(ip);
          };
          if (parser.Found("r") || parser.Found("y"))
          {//TCP IP client interface
              CInterfaceProperties ip;
              ip.m_it=TcpIpClient;
              ip.m_bInput=parser.Found("r");
              ip.m_bOutput=parser.Found("y");
              ip.m_bEcho=parser.Found("d");
              ip.m_bReopenClosedPipe=true;
              ip.m_bDuplex=false;
              parser.Found("tcpceolin",&sName);
              ip.m_sEolIn=GetEol(parser.Found("unix_eol"),parser.Found("tcpceolin",&sName),sName.c_str());
              parser.Found("tcpceolout",&sName);
              ip.m_sEolOut=GetEol(parser.Found("unix_eol"),parser.Found("tcpceolout",&sName),sName.c_str());
              ip.m_bNoErrors=parser.Found("tcpcnerr");
              if(!parser.Found("t",&ip.m_nPort))
                  throw std::string("Missing tcp ip client port in command line args");
              if(!parser.Found("a",&sName))
                  throw std::string("Missing tcp ip client address in command line args");
              ip.m_sAddress=sName;
              if(parser.Found("exp_time",&ip.m_nExpTime))
                  ip.m_bExp=true;
              if(parser.Found("tcpcnewsession")) ip.m_bOption2=true;
              else ip.m_bOption2=false;
              if(parser.Found("tcpcto",&sName))ip.m_sTo=sName;
              if(parser.Found("tcpcfrom",&sName))ip.m_sFrom=sName;
              vip.push_back(ip);
          };
          if (parser.Found("tcpc2in") || parser.Found("tcpc2out"))
          {//TCP IP 2 client interface
              CInterfaceProperties ip;
              ip.m_it=TcpIpClient;
              ip.m_bInput=parser.Found("tcpc2in");
              ip.m_bOutput=parser.Found("tcpc2out");
              ip.m_bEcho=parser.Found("tcpc2echo");
              ip.m_bReopenClosedPipe=true;
              ip.m_bDuplex=false;
              parser.Found("tcpc2eolin",&sName);
              ip.m_sEolIn=GetEol(parser.Found("unix_eol"),parser.Found("tcpc2eolin",&sName),sName.c_str());
              parser.Found("tcpc2eolout",&sName);
              ip.m_sEolOut=GetEol(parser.Found("unix_eol"),parser.Found("tcpc2eolout",&sName),sName.c_str());
              ip.m_bNoErrors=parser.Found("tcpc2nerr");
              if(!parser.Found("tcpc2port",&ip.m_nPort))
                  throw std::string("Missing tcp ip 2 client port in command line args");
              if(!parser.Found("tcpc2address",&sName))
                  throw std::string("Missing tcp ip client address in command line args");
              ip.m_sAddress=sName;
              if(parser.Found("exp_time",&ip.m_nExpTime))
                  ip.m_bExp=true;
              if(parser.Found("tcpc2newsession")) ip.m_bOption2=true;
              else ip.m_bOption2=false;
              if(parser.Found("tcpc2to",&sName))ip.m_sTo=sName;
              if(parser.Found("tcpc2from",&sName))ip.m_sFrom=sName;
              vip.push_back(ip);
          };
          if (parser.Found("f") || parser.Found("g"))
          {//Serial Interface
              CInterfaceProperties ip;
              ip.m_it=Serial;
              ip.m_bInput=parser.Found("f");
              ip.m_bOutput=parser.Found("g");
              ip.m_bEcho=parser.Found("j");
              ip.m_bReopenClosedPipe=true;
              ip.m_bDuplex=false;
              parser.Found("sereolin",&sName);
              ip.m_sEolIn=GetEol(parser.Found("unix_eol"),parser.Found("sereolin",&sName),sName.c_str());
              parser.Found("sereolout",&sName);
              ip.m_sEolOut=GetEol(parser.Found("unix_eol"),parser.Found("sereolout",&sName),sName.c_str());
              if(!parser.Found("k",&ip.m_nPort))
                  throw std::string("Missing serial baud in command line args");
              if(!parser.Found("l",&sName))
                  throw std::string("Missing serial comm in command line args");
              ip.m_sAddress=sName;
              if(parser.Found("serto",&sName))ip.m_sTo=sName;
              if(parser.Found("serfrom",&sName))ip.m_sFrom=sName;
              if(parser.Found("sersplit",&ip.m_nSplitLines));
              vip.push_back(ip);
          };
          break;
      default:
         throw std::string("Incorrect command line syntax");
         break;
   };
   return vip;
}

void CMultiplexor::ReceiveString(std::string s, CInterfaceProperties ip)
{
   std::pair<std::string,CInterfaceProperties> pr(s,ip);
   if (g_bVerbose) g_ui.UpdateStatus(ip.GetExtendedType()+" received: [" 
      + s + "] : " + CUserInterface::GetCharCodes(s+ip.m_sEolIn));
   if (s.substr(0,g_sName.length())!=g_sName)
      m_sbuf.Add(pr);
   else if(s==(g_sName+" list interfaces"))
   {//List interfaces
      wxMutexLocker singleLock(m_CritSection);
      if (!singleLock.IsOk()) throw std::string("wxMutexLocker failed");
      g_ui.UpdateStatus("List of interfaces:");
      for(int i=0;i<m_vpife.size();i++)
         m_vpife[i]->m_ip.Print();
   }
   //Execute System Command - disable for secure builds
   //else if (s.substr(0,g_sName.length()+7)==(g_sName+" system "))
   //   std::system(s.substr(g_sName.length()+7).c_str());
   //Execute interface command
   else if(s.substr(0,g_sName.length()+1)==g_sName+":")
   {
      wxMutexLocker singleLock(m_CritSection);
      if (!singleLock.IsOk()) throw std::string("wxMutexLocker failed");
      for(int i=0;i<m_vpife.size();i++)
         if(s.substr(0,g_sName.length()+m_vpife[i]->m_ip.GetType().length()+2)==g_sName+":"+m_vpife[i]->m_ip.GetType()+" ")
            m_vpife[i]->m_sbufCmd.Add(s.substr(g_sName.length()+m_vpife[i]->m_ip.GetType().length()+2,std::string::npos));
   }
   //Parse and execute command line parameters
   else if(s.substr(0,g_sName.length()+5)==g_sName+" cmd ")
   {
      g_ui.UpdateStatus("Parsing and executing command line command");
      g_ui.UpdateStatus(s);
      std::vector<std::string> vs;
      vs.push_back("RC.exe");//need one empty string upfront
      std::istringstream iss(s.substr(g_sName.length()+5,std::string::npos));
      while (!iss.eof())
      {
         std::string sub;
         iss >> sub;
         if(!sub.empty())
            vs.push_back(sub);
      }
      char** argv=new char* [vs.size()];
      for(int i=0;i<vs.size();i++)
      {
         argv[i]=new char[vs[i].length()+1];
         strcpy(argv[i],vs[i].c_str());
      }
      std::vector<CInterfaceProperties> ip;
      try
      {
         ip=ParseCommandLine(vs.size(),argv);
      }
      catch(std::string s)
      {
         g_ui.LogError(s);
      }
      for(int i=0;i<vs.size();i++)
         delete[] argv[i];
      delete[] argv;
      for(int i=0;i<ip.size();i++)
         AddInterface(ip[i]);
   }
   else m_sbuf.Add(pr);
}

void CMultiplexor::AddTcpIpServerSessionInterface(/*CInterface*/void* p_ife)
{
   wxMutexLocker singleLock(m_CritSection);
   if (!singleLock.IsOk()) throw std::string("wxMutexLocker failed");
   m_vpife.push_back((CTcpIpServerSessionInterface*)p_ife);
   m_vpife.back()->Init();
}

void CMultiplexor::DeleteDisconnectedInterfaces()
{
   for(int i=0;i<m_vpife.size();i++)
   {
      if (m_vpife[i]->m_bQuit==true && !m_vpife[i]->ThreadsRunning())
      {
         wxMutexLocker singleLock(m_CritSection);
         if (!singleLock.IsOk()) throw std::string("wxMutexLocker failed");
         g_ui.UpdateStatus("Deleted disconnected interface:"+m_vpife[i]->m_ip.GetExtendedType());
         delete m_vpife[i];
         m_vpife.erase(m_vpife.begin()+i);
         i--;
         //Find out if there is a TcpIpServer and no TcpIpServerSession 
         for(int j=0;j<m_vpife.size();j++)
         {
            if (m_vpife[j]->m_ip.m_it==TcpIpServer)
            {
               bool bFound=false;
               for (int n=0;n<m_vpife.size();n++)
                  if(m_vpife[n]->m_ip.m_it==TcpIpServerSession)
                     bFound=true;
               //suspend output till all sessions are gone
               m_vpife[j]->m_ip.m_bSuspendOutput=bFound;
            }
         }
      }
   }
   if(m_vpife.size()==0)
   {
      g_ui.UpdateStatus("CMultiplexor::DeleteDisconnectedInterfaces is shutting down program execution because there are no interfaces to service");
      g_bQuit=true;
   }
}

void CMultiplexor::Broadcast()
{
   DeleteDisconnectedInterfaces();
   if(!m_sbuf.IsEmpty())
   {
      std::pair<std::string,CInterfaceProperties> pr=m_sbuf.Get();
	  std::string sDestTag = GetDestTag(pr.first);///WRITE THIS FUNCTION MIKE!!!
      m_nSleep=0;
      for(int i=0;i<m_vpife.size();i++)
      {
         if (!g_bQuit && !(m_vpife[i]->m_ip.Equal(pr.second) && m_vpife[i]->m_ip.m_bEcho==false)
			 && m_vpife[i]->m_ip.m_sTo==sDestTag )
         {
            try
            {
               if(m_vpife[i]->m_ip.m_bOutput && !m_vpife[i]->m_ip.m_bSuspendOutput && !m_vpife[i]->m_bQuit)
                  m_vpife[i]->m_sbuf.Add(pr);
            }
            catch(std::string sError)
            {
               g_ui.LogError("CMultiplexor::Broadcast caught exeception: "+sError);
            }
         }
      }
   }
   if(m_nSleep!=0)
   {
      if(m_nSleep>20)m_nSleep=20;
      wxMilliSleep(m_nSleep);
   }
   m_nSleep++;
}



CMultiplexor::CMultiplexor():m_sbuf(g_nBufSize,100,"",false,0),m_nSleep(0)
{
}

CMultiplexor::~CMultiplexor()
{
   g_ui.UpdateStatus("Shutting down CMultiplexor::~CMultiplexor");
   for(int i=0;i<m_vpife.size();i++) m_vpife[i]->m_bQuit=true;
   for(int i=0;i<m_vpife.size();i++)
   {
      g_ui.UpdateStatus("Waiting for interface to stop: "+m_vpife[i]->m_ip.GetExtendedType());
      while(m_vpife[i]->ThreadsRunning())
         wxMilliSleep(10);
      g_ui.UpdateStatus("Deleting interface: "+m_vpife[i]->m_ip.GetExtendedType());
      delete m_vpife[i];
   }
}

void CMultiplexor::AddInterface(CInterfaceProperties ip)
{
   if (g_bQuit==true) return;
   wxMutexLocker singleLock(m_CritSection);
   if (!singleLock.IsOk()) throw std::string("wxMutexLocker failed");
   switch(ip.m_it)
   {
   case StdIO:
      m_vpife.push_back(new CStdInterface(this,ip));
      break;
   case TcpIpServer:
      m_vpife.push_back(new CTcpIpServerInterface(this,ip));
      break;
   case TcpIpClient:
      m_vpife.push_back(new CTcpIpClientInterface(this,ip));
      break;
   case Serial:
      m_vpife.push_back(new CSerialPortInterface(this,ip));
      break;
	case Log:
      m_vpife.push_back(new CLogInterface(this,ip));
      break;
	case File:
      m_vpife.push_back(new CFileInterface(this,ip));
      break;
	case Test:
   	m_vpife.push_back(new CTestInterface(this,ip));
      break;
    default:
      throw std::string("CMultiplexor::AddInterface found unknown class");
      break;
    }
    m_vpife.back()->Init();
}
