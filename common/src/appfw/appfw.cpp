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

#include <opencraft/versiondefs.h>
#include <iostream>

#include <opencraft/appfw/console/logging/simple_logger.h>
#include <opencraft/appfw/console/base_console.h>
#include <opencraft/appfw/appstate/fsm/base_fsm.h>
#include <opencraft/appfw/appstate/fsm/idle_state.h>

namespace opencraft { namespace appfw {

App::App() {
     std::cout << "OpenCraft App framework compiled with " << opencraft_framework_longver()  << std::endl;
     std::cout << "Running on "                            << OPENCRAFT_FRAMEWORK_LONG_VER   << std::endl;

     std::cout << OPENCRAFT_FRAMEWORK_COPYRIGHT << std::endl;

     this->SetDebugMode(false);

     // create the console and logger
     this->Console = new console::BaseConsole();
     this->Logger  = new console::logging::SimpleLogger(this->Console); // spits out logs to the console only
     this->Logger->SetDebugMode(this->debug_mode);

     // create the FSM and add default idle state
     this->FSM     = new appstate::fsm::BaseFSM();
     this->FSM->AddState(new appstate::fsm::IdleState(this->FSM));
}

void App::run() {
     for(;;) { // run forever!
        // TODO: Gareth input from UIs here and turn into events for the app state
        this->FSM->Update(); // update the current app state
        for(auto const& ui: this->UserInterfaces) {
            // TODO: Update UI states here
        }
        for(auto const& ui: this->UserInterfaces) {
            // TODO: render UI states here (where relevant)
        }
     }
}

void App::SetDebugMode(bool _debug_mode) {
     this->debug_mode = _debug_mode;
     this->Logger->SetDebugMode(_debug_mode);
}

bool App::GetDebugMode() {
     return this->debug_mode;
}

void App::AddInterface(interfaces::BaseInterface* _interface) {
     this->UserInterfaces.push_back(_interface);
}

}};
