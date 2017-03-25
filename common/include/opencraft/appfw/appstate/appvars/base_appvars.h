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
//     BaseAppVars class
//
//-----------------------------------------------------------------------------

#pragma once

#include <opencraft/common.h>

#include <map>
#include <string>

// getters and setters to bind to an appvar
typedef void        (*s_setter_t)(std::string s);
typedef std::string (*s_getter_t)();

namespace opencraft { namespace appfw {
   namespace appstate { namespace appvars {

   class BaseAppVar {
      public:
         BaseAppVar(std::string val);
//         BaseAppVar(s_setter_t setter, s_getter_t getter); // TODO: implement function binding
         std::string get();
         void        set(std::string s);
      protected:
         std::string s_val;
   };

   class BaseAppVars {
      public:
         BaseAppVars();
         std::string get(std::string k);
         void set(std::string k, std::string v);

      protected:
         std::map<std::string,BaseAppVar*> vars;
   };

}}}};
