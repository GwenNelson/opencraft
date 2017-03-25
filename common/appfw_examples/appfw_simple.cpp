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
//     Simple example for the app framework
//
//-----------------------------------------------------------------------------

#include <opencraft/appfw/appfw.h>
#include <opencraft/appfw/interfaces/cli/simple_cli.h>
#include <opencraft/appfw/appstate/fsm/base_state.h>

#include <opencraft/appfw/appstate/fsm/base_fsm.h>


using namespace opencraft;

class SimpleStartState : public appfw::appstate::fsm::BaseState {
   public:
      SimpleStartState(appfw::appstate::fsm::BaseFSM *_FSM) : BaseState(_FSM) {};
      void        Init();   // override the Init() - this is NOT the same as the C++ constructor and you should NOT override the constructor
      void        Update(); // override the Update()
      const char* GetName() { return "SimpleStartState";}
};

void SimpleStartState::Init() {
     OC_LOG_INFO(this->FSM->app,"Hello from SimpleStartState");
     OC_LOG_INFO(this->FSM->app,"On my first update i'll just die");
}

void SimpleStartState::Update() {
     OC_LOG_INFO(this->FSM->app,"Now i'm just gonna die");
     this->FSM->Switch("EndProgramState"); // EndProgramState is a state that is setup for us by the framework
}

int main(int argc, char** argv) {
    // create an app instance
    appfw::App *SimpleApp = new appfw::App();

    // turn on debug mode - this will make the logger spit out info on state changes and other useful stuff
    SimpleApp->SetDebugMode(true); // this same method is used by appvars

    // setup CLI for the user interface - this is NOT added by default since some apps might not want it
    // without a CLI interface the console buffer won't be sent to stdout and that means we'll not see logs
    // we don't reconfigure logging here as the default (SimpleLogger) works fine
    SimpleApp->AddInterface(new appfw::interfaces::cli::BaseCLI(SimpleApp));

    // at this point the CLI interface will dump the current console buffer to stdout, this is by design and is intended to allow CLI
    // users to see debug logs from before the CLI was created - in practice this is usually pretty empty

    // add an FSM state - not doing this means the main loop will simply idle forever
    SimpleApp->FSM->AddState(new SimpleStartState(SimpleApp->FSM));

    // set the initial state (which is by default an instance of appfw::appstate::fsm::IdleState) and then run the app
    SimpleApp->FSM->Switch("SimpleStartState"); // to set initial state we just switch to it, this will cause switch from idle to start state
    SimpleApp->run();
}

