//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of OpenCraft.
//
// The OpenCraft client is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// The OpenCraft client is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenCraft.  If not, see <http://www.gnu.org/licenses/>.
//
// DESCRIPTION:
//     Testing configuration
//
//-----------------------------------------------------------------------------

#include <opencraft/appfw/appfw.h>
#include <opencraft/appfw/interfaces/cli/simple_cli.h>
#include <opencraft/appfw/console/logging/base_logger.h>
#include <opencraft/appfw/console/cmdshell/base_shell.h>


#include <iostream>


#include <opencraft/appfw/configuration/config_manager.h>

using namespace opencraft;

// this program reads configuration settings from either the command line or a file named appfw_config.cfg
// the command line overrides the config file

// the settings are as follows:
//    username (-U|--username) - mandatory, bound to a read/write GlobalVar
//    listen_port (-P|--port)  - optional, bound to a read-only GlobalVar
// a default is provided for both settings


int main(int argc, char** argv) {
    // boilerplate init stuff
    appfw::App *SimpleApp = new appfw::App();


    // create config manager
    appfw::configuration::ConfigManager *cfg = new appfw::configuration::ConfigManager({"."},SimpleApp);
    
    // setup the variables
    cfg->add_string("username",   "SirGareth","U,username","Username to use");
    cfg->add_int   ("listen_port",25565,      "P,port","Port to listen on");

    // load the config file if it exists
    cfg->load("appfw_config.cfg");

    // even if it doesn't exist, save the config file
    cfg->save("appfw_config.cfg");


    // do this just to dump output to stdout
    SimpleApp->AddInterface(new appfw::interfaces::cli::BaseCLI(SimpleApp));


    appfw::console::cmdshell::BaseShell *SimpleShell = new appfw::console::cmdshell::BaseShell(SimpleApp);


    SimpleApp->Console->add_output("\nTry typing set to see the loaded settings\n\n");

    SimpleApp->run();

}

