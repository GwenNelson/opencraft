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
//     BaseFSM class
//
//-----------------------------------------------------------------------------

#include <opencraft/appfw/appstate/fsm/base_fsm.h>
#include <opencraft/appfw/appfw.h>

namespace opencraft { namespace appfw { namespace appstate { namespace fsm {

BaseFSM::BaseFSM(opencraft::appfw::App* _app) {
     this->app = _app;
     this->cur_state = NULL;
}

void BaseFSM::Update() {
     if(this->cur_state != NULL) {
       this->cur_state->Update();
     }
}

void BaseFSM::Switch(std::string state_name) {
     if(this->states.find(state_name) != this->states.end()) {
        if(this->cur_state == NULL) {
          this->app->Logger->info(std::string("Starting with state ") + state_name);
        } else {
          this->app->Logger->info(std::string("Switching state from") + this->cur_state->GetName() + std::string(" to ") + state_name);
//        this->cur_state->Shutdown(); // TODO: implement this
        }
        this->cur_state = this->states[state_name];
     } else {
        this->app->Logger->info(std::string("Could not switch to unknown state: ") + state_name);
     }
}

void BaseFSM::AddState(BaseState *state) {
     this->states[state->GetName()] = state;
     state->Init();
}



}}}};
