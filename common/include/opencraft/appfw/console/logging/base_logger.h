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
//     BaseLogger class
//
//-----------------------------------------------------------------------------

#pragma once

#include <opencraft/common.h>

#include <opencraft/appfw/console/base_console.h>

#include <string>

namespace opencraft { namespace appfw { namespace console { namespace logging {

   class BaseLogger {
      public:
         BaseLogger(opencraft::appfw::console::BaseConsole *_Console);
         void fmt_output(std::string sev_s, std::string msg);
         void info(std::string s);
         void debug(std::string s);

      private:
         opencraft::appfw::console::BaseConsole *Console;
         bool debug_mode;
   };

}}}};

enum log_severity {
     info,
     debug,
     error,
     warn
};


