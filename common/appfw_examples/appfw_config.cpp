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

#include <iostream>

using namespace opencraft;

// this program reads configuration settings from either the command line or a file named appfw_config.cfg
// the command line overrides the config file

// the settings are as follows:
//    username (-U|--username) - mandatory, bound to a read/write GlobalVar
//    listen_port (-P|--port)  - optional, bound to a read-only GlobalVar
// a default is provided for both settings

// when run, the program will simply dump the settings to stdout

// for simplicity sake, we don't actually run the app in this example

int main(int argc, char** argv) {
    // boilerplate init stuff
    appfw::App *SimpleApp = new appfw::App();
    SimpleApp->AddInterface(new appfw::interfaces::cli::BaseCLI(SimpleApp));

    appfw::configuration *ConfigManager cfg = new appfw::configuration::ConfigManager(SimpleApp);

    // specify our settings
    cfg->add_option("username",     // variable name
                    "U,username",   // command-line switch/flag
                    true,           // bind to a GlobalVar
                    false,          // make the GlobalBar read only
                    "SirGareth",    // default value                  
                    true);          // required value
    cfg->add_option("listen_port",  // variable name
                    "P,port",       // command-line switch/flag
                    true,           // bind to a GlobalVar
                    true,           // make the GlobalBar read only
                    "25565",        // default value                  
                    false);         // required value

    // load the config file if it exists, otherwise generate it and write it out
    cfg->load_or_generate("./appfw_config.cfg");
    
    // parse command line options
    cfg->parse_cmdline(argc,argv);

    // dump the settings
    std::cout << "username:    " << SimpleApp->FSM->GlobalVars->get("username")    << std::endl;
    std::cout << "listen port: " << SimpleApp->FSM->GlobalVars->get("listen_port") << std::endl;
}

