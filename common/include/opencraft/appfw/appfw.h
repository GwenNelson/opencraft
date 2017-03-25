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
//     Definitions for the app framework
//
//-----------------------------------------------------------------------------

#pragma once

#include <opencraft/common.h>

#include <vector>

#include <opencraft/appfw/console/logging/base_logger.h>
#include <opencraft/appfw/console/base_console.h>

#include <opencraft/appfw/appstate/fsm/base_fsm.h>


namespace opencraft { namespace appfw {
   namespace interfaces { class BaseInterface; };
   class App {
      public:
         App();

         void run(); // if this returns, the program should die
      
         // framework components         
         console::logging::BaseLogger            *Logger;
         console::BaseConsole                    *Console;
         appstate::fsm::BaseFSM                  *FSM;
         std::vector<interfaces::BaseInterface*> UserInterfaces;
         
         // used by appvars, but can also be directly invoked
         void SetDebugMode  (bool _debug_mode);
         bool GetDebugMode  ();

         void AddInterface  (interfaces::BaseInterface* _interface);
     private:
         bool debug_mode;
   };

}};
