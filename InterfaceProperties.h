///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

#ifndef INTERFACETYPE_H
#define INTERFACETYPE_H

#include <string>

enum InterfaceType //prefix it
{
    Serial=0,
    TcpIpClient,
    TcpIpServer,
    StdIO,
	 Log,
    TcpIpServerSession,
    File,
    Test
};

class CInterfaceProperties //preffix ip
{
public:
    CInterfaceProperties(void);
    ~CInterfaceProperties(void);
	 void Print();
	 std::string GetType();
    std::string GetExtendedType();
    bool Equal (CInterfaceProperties& ip);
    bool Match(CInterfaceProperties& ip);
    InterfaceType   m_it;//Type of interface
    long            m_nPort;//tcpip port
    std::string     m_sAddress;//tcpip address or file name
    std::string     m_sPeerAddress;
    long            m_nPeerPort;
    bool            m_bOutput;//interface supports output (can write data to interface)
    bool            m_bSuspendOutput;
    bool            m_bInput;//interface supports input (can read data from interface)
    bool            m_bReopenClosedPipe;
    bool            m_bEcho;
    bool            m_bDuplex;
    std::string     m_sEolIn;
    std::string     m_sEolOut;
    bool            m_bNoErrors;
    bool            m_bOption1;
    bool            m_bOption2;
    bool            m_bOption3;
    long            m_nExpTime;//Number of seconds before message expires
    bool            m_bExp;//Determines whether to purge expired messages
    std::string     m_sTo;//Send data to interfaces that have matching characters
    std::string     m_sFrom;//Receive data from interfaces that have matching characters
    long            m_nSplitLines;//Split lines by #of characters (0 is false)
};

#endif//INTERFACETYPE_H
