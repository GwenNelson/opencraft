//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of the OpenCraft client.
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
//     Implementation of the App class
//
//-----------------------------------------------------------------------------

#include <opencraft/appfw/appfw.h>
#include <opencraft/appfw/interfaces/base_interface.h>

#include <opencraft/versiondefs.h>
#include <iostream>

#include <opencraft/appfw/console/base_console.h>
#include <opencraft/appfw/appstate/fsm/base_fsm.h>
#include <opencraft/appfw/appstate/fsm/idle_state.h>
#include <opencraft/appfw/appstate/fsm/end_program_state.h>

namespace opencraft { namespace appfw {

App::App() {
     std::cout << "OpenCraft App framework compiled with " << opencraft_framework_longver()  << std::endl;
     std::cout << "Running on "                            << OPENCRAFT_FRAMEWORK_LONG_VER   << std::endl;

     std::cout << OPENCRAFT_FRAMEWORK_COPYRIGHT << std::endl << std::endl;

     this->SetDebugMode(false);

     // create the console and logger
     this->Console = new console::BaseConsole();
     this->Logger  = new console::logging::BaseLogger(this->Console); // spits out logs to the console only
     this->Logger->info("Logging begins");

     // create the FSM and add default states
     this->FSM     = new appstate::fsm::BaseFSM(this);
     this->FSM->AddState(new appstate::fsm::IdleState(this->FSM));
     this->FSM->AddState(new appstate::fsm::EndProgramState(this->FSM));
     this->FSM->Switch("IdleState");
}

void App::run() {
     // basic main loop overview:
     // 1 - ask all active UIs to grab events (user input comes in here)
     //     TODO: network I/O stuff
     // 2 - each UI grabs events and processes them, updating relevant state directly if needed
     //     some UIs have listeners and will send updates to their listeners instead of touching state depending on the event
     // 3 - current FSM state is asked to update
     // 4 - each UI in turn is asked to update any required state
     // 5 - each UI in turn is asked to render
     for(;;) { // run forever!

        for(auto const& ui: this->UserInterfaces) {
            ui->GrabEvents(); // ask UIs to grab events and update listeners
        }

        this->FSM->Update(); // update the current app state

        for(auto const& ui: this->UserInterfaces) {
            ui->Update(); // ask UIs to update their own internal state
        }

        for(auto const& ui: this->UserInterfaces) {
            ui->Render(); // ask UIs to render their internal state to the user
        }
     }
}

void App::SetDebugMode(bool _debug_mode) {
     this->debug_mode = _debug_mode;
}

bool App::GetDebugMode() {
     return this->debug_mode;
}

void App::AddInterface(interfaces::BaseInterface* _interface) {
     this->UserInterfaces.push_back(_interface);
}

}};
