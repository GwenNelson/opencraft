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
//     BaseConsole class
//
//-----------------------------------------------------------------------------

#pragma once

#include <opencraft/common.h>
#include <vector>
#include <string>

namespace opencraft { namespace appfw { namespace console {

   class BaseConsole {
      public:
         BaseConsole();
         
         // basic operations on output
         void clear_output();            // clear the output buffer, this will cause scrollback to disappear
         void add_output(std::string s); // append a string to the output buffer
         std::string get_output();       // return the current contents of the output buffer

         // basic operations on input
         // TODO: add input line history etc
         void clear_input();                    // clear the input buffer
         void add_input(std::string);           // append a line read from the user to the input buffer where the UI can read it
         std::vector<std::string> get_input();  // return the current contents of the input buffer

   };

}}};
