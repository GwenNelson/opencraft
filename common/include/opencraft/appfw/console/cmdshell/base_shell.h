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
//     BaseShell class
//
//-----------------------------------------------------------------------------

#pragma once

#include <opencraft/common.h>
#include <vector>
#include <map>
#include <string>

#include <opencraft/appfw/console/console_event_listener.h>
#include <opencraft/appfw/appfw.h>

#include <boost/program_options.hpp>

namespace po = boost::program_options;



namespace opencraft { namespace appfw { namespace console { namespace cmdshell {
   class BaseShell;
   typedef void (*cmdshell_cmd_t)(po::variables_map vm,opencraft::appfw::App* app,BaseShell *shell);
   class BaseShell : public opencraft::appfw::console::ConsoleEventListener {
      public:
         BaseShell(opencraft::appfw::App* _app);
         void add_cmd(std::string cmdname, std::string basic_usage, std::string full_usage, cmdshell_cmd_t cmdfunc, po::options_description *desc, po::positional_options_description* pos_opts_desc);
         void on_output(std::string s);
         void on_output_clear();
         void on_input(std::string s);
         void on_input_clear();

         opencraft::appfw::App* app;
         std::map<std::string,po::options_description*>            cmd_opts;
         std::map<std::string,po::positional_options_description*> cmd_pos_opts;
         std::map<std::string,cmdshell_cmd_t>                      cmd_funcs;
         std::map<std::string,std::string>                         cmd_basic_usage;
         std::map<std::string,std::string>                         cmd_full_usage;
   };

}}}};
