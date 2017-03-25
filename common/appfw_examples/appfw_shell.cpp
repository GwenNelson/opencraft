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
//     Testing command line shell
//
//-----------------------------------------------------------------------------

#include <opencraft/appfw/appfw.h>
#include <opencraft/appfw/interfaces/cli/simple_cli.h>
#include <opencraft/appfw/appstate/fsm/base_state.h>

#include <opencraft/appfw/appstate/fsm/base_fsm.h>

#include <opencraft/appfw/console/logging/base_logger.h>
#include <opencraft/appfw/console/cmdshell/base_shell.h>

using namespace opencraft;

// this program only really uses IdleState, this works fine if you're not using the FSM
// to setup a simple command-line shell we just create an instance of cmdshell and pass the app to the constructor
// by default the shell comes with a couple of commands: set, help, quit

int main(int argc, char** argv) {
    // boilerplate init stuff
    appfw::App *SimpleApp = new appfw::App();
    SimpleApp->SetDebugMode(true);
    SimpleApp->AddInterface(new appfw::interfaces::cli::BaseCLI(SimpleApp));

    // create our command-line shell, BaseShell class will handle adding itself to the console etc
    appfw::console::cmdshell::BaseShell *SimpleShell = new appfw::console::cmdshell::BaseShell(SimpleApp);

    // tell the user to start using the shell with a welcome message and a prompt sent to the console
    SimpleApp->Console->add_output("\nShell is ready! type help for help\n\n");
    SimpleApp->Console->add_output("\n> ");
    // no need to set a state, IdleState is fine
    SimpleApp->run();
}

