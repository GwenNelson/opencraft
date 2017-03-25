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
//     BaseState class
//
//-----------------------------------------------------------------------------

#pragma once

#include <opencraft/common.h>

#include <map>
#include <string>

#include <opencraft/appfw/appstate/fsm/base_state.h>
#include <opencraft/appfw/appstate/fsm/base_fsm.h>

namespace opencraft { namespace appfw { namespace appstate { namespace fsm {

   class EndProgramState : public BaseState {
      public:
         EndProgramState(BaseFSM *_FSM) : BaseState(_FSM) {};
         void        Init();
         void        Update();
         const char* GetName();
   };

}}}};
