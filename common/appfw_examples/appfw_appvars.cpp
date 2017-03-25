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
//     Testing appvars functionality
//
//-----------------------------------------------------------------------------

#include <opencraft/appfw/appfw.h>
#include <opencraft/appfw/interfaces/cli/simple_cli.h>
#include <opencraft/appfw/appstate/fsm/base_state.h>

#include <opencraft/appfw/appstate/fsm/base_fsm.h>

#include <opencraft/appfw/console/logging/base_logger.h>

using namespace opencraft;

// we have 2 states - AppVarSetState sets an appvar, AppVarGetState reads it
// to make things simple, AppVarSetState will jump straight to AppVarGetState after setting in the first update
// it is a stupid and bad idea to switch state in Init()
// so the program flow is like this: startup -> IdleState -> AppVarSetState -> AppVarGetState -> EndProgramState

class AppVarSetState : public appfw::appstate::fsm::BaseState {
   public:
      AppVarSetState(appfw::appstate::fsm::BaseFSM *_FSM) : BaseState(_FSM) {};
      void        Init();
      void        Update();
      const char* GetName() { return "AppVarSetState";}
};

void AppVarSetState::Init() {
     this->FSM->GlobalVars->set("testvar","hello there");
     OC_LOG_INFO(FSM->app,"I just set global variable testvar to \"hello there\"");

}

void AppVarSetState::Update() {
     this->FSM->LocalVars->set("testvar","a different value!");
     OC_LOG_INFO(FSM->app,"I just set local variable testvar to \"a different value!\"");
     OC_LOG_INFO(FSM->app,std::string("global testvar=\"") + this->FSM->GlobalVars->get("testvar") + std::string("\""));
     OC_LOG_INFO(FSM->app,std::string("local testvar=\"")  + this->FSM->LocalVars->get("testvar") + std::string("\""));
     OC_LOG_INFO(FSM->app,"Now let's read that variable using AppVarGetState");
     this->FSM->Switch("AppVarGetState");
}

class AppVarGetState : public appfw::appstate::fsm::BaseState {
   public:
      AppVarGetState(appfw::appstate::fsm::BaseFSM *_FSM) : BaseState(_FSM) {};
      void        Init();
      void        Update();
      const char* GetName() { return "AppVarGetState";}
};

void AppVarGetState::Init() {
     // remember, Init() is only called when added to the FSM
     OC_LOG_INFO(this->FSM->app,"AppVarGetState::Init() - I just got added to the FSM, but i'm probably not running yet");
}

void AppVarGetState::Update() {
     OC_LOG_INFO(FSM->app,std::string("global testvar=\"") + this->FSM->GlobalVars->get("testvar") + std::string("\""));
     OC_LOG_INFO(FSM->app,std::string("local testvar=\"")  + this->FSM->LocalVars->get("testvar") + std::string("\""));
     OC_LOG_INFO(this->FSM->app,"Now i'm going to kill the program");
     this->FSM->Switch("EndProgramState");
}

int main(int argc, char** argv) {
    // boilerplate init stuff
    appfw::App *SimpleApp = new appfw::App();
    SimpleApp->SetDebugMode(true);
    SimpleApp->AddInterface(new appfw::interfaces::cli::BaseCLI(SimpleApp));

    // add our states    
    SimpleApp->FSM->AddState(new AppVarSetState(SimpleApp->FSM));
    SimpleApp->FSM->AddState(new AppVarGetState(SimpleApp->FSM));

    // set the initial state and run
    SimpleApp->FSM->Switch("AppVarSetState");
    SimpleApp->run();
}

