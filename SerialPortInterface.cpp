///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

//Class declaration
#include "SerialPortInterface.h"

//STL
#include <iostream>
#include <stdexcept>
#include <ctime>
//wxWidgets
#include "wx\utils.h"

CSerialPortInterface::CSerialPortInterface(CInterfaceCallBack* picb,CInterfaceProperties ip)
:CBlockDataInterface(picb,ip)
{
}

CSerialPortInterface::~CSerialPortInterface(void)
{
    m_wxsp.Close();
}

std::pair<std::string,bool> CSerialPortInterface::GetString()
{
    //Read data from serial port to buffer
    const int READBUFFER=100;
    char sz[READBUFFER+1];
    int n=m_wxsp.Read(sz,READBUFFER);
    if (n>0)
    {
        sz[n]=0;
        AddData(sz);
    };
    //Return data from buffer
    if(!BufferEmpty()) return std::pair<std::string,bool>(GetStringFromBuffer(),true);
    return std::pair<std::string,bool>("",false);
}

void CSerialPortInterface::OpenPipe()
{
    CBlockDataInterface::OpenPipe();
    // port initialisation
    wxSerialPort_DCS wxdcs;
    wxdcs.parity = wxPARITY_NONE;
    wxdcs.baud=(wxBaud)m_ip.m_nPort;
    wxdcs.wordlen = 8;
    wxdcs.stopbits = 1;
    wxdcs.rtscts = false;
    wxdcs.xonxoff = false;
    if(m_wxsp.Open(m_ip.m_sAddress.c_str(),&wxdcs)== -1) //try to open the given port
        throw std::string("CSerialPortInterface::OpenPipe cannot open com port");
}

bool CSerialPortInterface::PipeClosed()
{
    return (!m_wxsp.IsOpen());
}

void CSerialPortInterface::Send(std::string sOut,std::string sInterfaceType)
{
    /*Write writes up to len bytes from the buffer starting with buf
    into the interface.
    \param buf start adress of the buffer
    \param len count of bytes, we want to write
    \return on success, the number of bytes written are returned 
    (zero indicates nothing was written).  On error, -1 is returned.*/
    char* sz=new char[sOut.length()+1];
    std::strcpy(sz,sOut.c_str());
    unsigned int i=m_wxsp.Write(sz,sOut.length());
    delete[] sz;
    if (sOut.length()!=i)
        throw std::string("CSerialPortInterface::Send can't write to serial port");
}
