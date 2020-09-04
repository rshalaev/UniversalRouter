///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

//wxWidgets
#include "wx/app.h"
#include "wx/utils.h"
#include "wx/socket.h"

//STL
#include <iostream>
#include <vector>

//Local dependancies
#include "global.h"
#include "multiplexor.h"

//Debug
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// PROGRAM OPERATION DESCRIPTION
// Implement Generic Interface functionality in Universal Router
// main function initializes an array of interfaces based on command line settings
// on call back the line gets sent to all interfaces

int main(int argc, char* argv[])
{
    try
    {
        //init wxWidgets
        wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");
        wxInitializer initializer;
        if (!initializer) throw std::string("Failed to initialize the wxWidgets library");
        wxSocketBase::Initialize();
        //init everything else
        std::vector<CInterfaceProperties> ip=CMultiplexor::ParseCommandLine(argc,argv);
        CMultiplexor mpx;
        g_ui.UpdateStatus("Starting interfaces");
        for(int i=0;i<ip.size();i++) mpx.AddInterface(ip[i]);
        //main loop
        g_ui.UpdateStatus("Running main program loop");
        while(!g_bQuit) mpx.Broadcast();
        //shutdown
	    return 0;
   }
   catch(std::string sError)
   {
        g_bQuit=true;
        g_ui.LogError("Main function caught exeception: " + sError);
        g_ui.UpdateStatus("Can't recover from the previous error, aborting execution");
        wxSleep(10);
        return 1;
   }
}
