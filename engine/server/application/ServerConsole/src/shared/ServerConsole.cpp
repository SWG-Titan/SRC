// ServerConsole.cpp
// Copyright 2000-02, Sony Online Entertainment Inc., all rights reserved. 
// Author: Justin Randall

//-----------------------------------------------------------------------

#include "FirstServerConsole.h"
#include "ConfigServerConsole.h"
#include "sharedFoundation/Os.h"
#include "sharedNetwork/Connection.h"
#include "sharedNetworkMessages/ConsoleChannelMessages.h"
#include "ServerConsole.h"
#include "ServerConsoleConnection.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>

//-----------------------------------------------------------------------

namespace ServerConsoleNamespace
{
    ServerConsoleConnection * s_serverConnection = 0;
    bool s_done = false;
}

using namespace ServerConsoleNamespace;

//-----------------------------------------------------------------------

ServerConsole::ServerConsole()
{
}

//-----------------------------------------------------------------------

ServerConsole::~ServerConsole()
{
}

//-----------------------------------------------------------------------

void ServerConsole::done()
{
    s_done = true;
}

//-----------------------------------------------------------------------

void ServerConsole::run()
{
    const char * address = ConfigServerConsole::getServerAddress();
    const uint16 port = ConfigServerConsole::getServerPort();

    if (!address || !port)
        return;

    // Connect immediately so we can reuse connection
    s_serverConnection = new ServerConsoleConnection(address, port);

    //-------------------------------------------------------------------
    // 1) Read piped stdin first (if any)
    //-------------------------------------------------------------------

    std::string input;
    char inBuf[1024];

    size_t bytesRead = 0;
    while ((bytesRead = fread(inBuf, 1, sizeof(inBuf), stdin)) > 0)
    {
        input.append(inBuf, bytesRead);
    }

    if (!input.empty())
    {
        ConGenericMessage msg(input);
        s_serverConnection->send(msg);
    }

    //-------------------------------------------------------------------
    // 2) Enter interactive mode
    //-------------------------------------------------------------------

    while (!s_done)
    {
        // Process network
        NetworkHandler::update();
        NetworkHandler::dispatch();

        // Non-blocking check for console input
        if (std::cin.good())
        {
            std::string line;
            if (std::getline(std::cin, line))
            {
                if (!line.empty())
                {
                    ConGenericMessage msg(line);
                    s_serverConnection->send(msg);
                }
            }
        }

        Os::sleep(1);
    }
}

//-----------------------------------------------------------------------