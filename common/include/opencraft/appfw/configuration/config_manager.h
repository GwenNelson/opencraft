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
//     ConfigManager class
//
//-----------------------------------------------------------------------------

#pragma once

#include <boost/program_options.hpp>

#include <opencraft/common.h>

#include <opencraft/appfw/appfw.h>

#include <string>
#include <vector>

#include <libconfig.h++>

namespace po = boost::program_options;

namespace opencraft { namespace appfw { namespace configuration {
   class ConfigManager {
      public:
         ConfigManager(std::vector<std::string> _search_paths, opencraft::appfw::App* _app);

         void add_bool   (std::string setting_name, bool        default_val, std::string cmdline_switch, std::string description);
         void add_int    (std::string setting_name, int         default_val, std::string cmdline_switch, std::string description);
         void add_string (std::string setting_name, std::string default_val, std::string cmdline_switch, std::string description);
         
         void parse_cmd(int argc, char** argv, std::string prog_desc); // handles command-line params, including usage info

         void init(int argc, char** argv); // attempt to load config file, autogenerating and saving if not found

         void load(std::string _filename);  // attempt to load config file, checking each of search_paths in order until found
         void save(std::string _filename);  // save config file, saves to first of search_paths by default, unless already loaded (in which case overwrites current)
         
         opencraft::appfw::App* app;
      protected:
         libconfig::Config        cfg;
         std::string              filename;     // full filename
         std::vector<std::string> search_paths;

         std::vector<std::string> bool_settings;
         std::vector<std::string> int_settings;
         std::vector<std::string> string_settings;
   };

}}};
