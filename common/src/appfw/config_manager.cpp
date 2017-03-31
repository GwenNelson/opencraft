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

#include <opencraft/common.h>

#include <opencraft/appfw/appfw.h>

#include <opencraft/appfw/configuration/config_manager.h>
#include <opencraft/appfw/console/logging/base_logger.h>


#include <string>
#include <vector>

#include <libconfig.h++>

#include <boost/filesystem.hpp>

using namespace libconfig;
using namespace boost::filesystem;

namespace opencraft { namespace appfw { namespace configuration {

ConfigManager::ConfigManager(std::vector<std::string> _search_paths, opencraft::appfw::App* _app) {
    this->app          = _app;
    this->search_paths = _search_paths;
    this->filename     = ""; // 0-length represents not specified or not found
}

void ConfigManager::add_bool(std::string setting_name, bool default_val, std::string cmdline_switch, std::string description) {
     Setting &root = this->cfg.getRoot();
     root.add(setting_name, Setting::TypeBoolean) = default_val;
     this->bool_settings.push_back(setting_name);
}

void ConfigManager::add_int(std::string setting_name, int default_val, std::string cmdline_switch, std::string description) {
     Setting &root = this->cfg.getRoot();
     root.add(setting_name, Setting::TypeInt) = default_val;
     this->int_settings.push_back(setting_name);
}

void ConfigManager::add_string(std::string setting_name, std::string default_val, std::string cmdline_switch, std::string description) {
     Setting &root = this->cfg.getRoot();
     root.add(setting_name, Setting::TypeString) = default_val;
     this->string_settings.push_back(setting_name);
}

void ConfigManager::parse_cmd(int argc, char** argv, std::string prog_desc) {

}

void ConfigManager::init(int argc, char ** argv) {
}

void ConfigManager::load(std::string _filename) {
     if(this->filename.length()==0) {
        for(const std::string& it : this->search_paths) {
            if(exists(it + "/" + _filename)) {
               this->filename = it + "/" + _filename;
            }
        }
     }
     if(!exists(this->filename)) {
        OC_LOG_ERROR(this->app,"Could not find " + _filename);
        return;
     }
     this->cfg.readFile(this->filename.c_str());
    
     Setting &root = this->cfg.getRoot();
     for(const std::string& it : this->bool_settings) {
         bool bv;
         root.lookupValue(it, bv);
         this->app->FSM->GlobalVars->set(it, bv);

     }

     for(const std::string& it : this->int_settings) {
         unsigned long long bv;
         root.lookupValue(it, bv);
         this->app->FSM->GlobalVars->set(it, (uint64_t)bv);

     }

     for(const std::string& it : this->string_settings) {
         std::string bs;
         root.lookupValue(it, bs);
         this->app->FSM->GlobalVars->set(it, bs);

     }

     OC_LOG_INFO(this->app,"Loaded configuration from " + this->filename);
}

void ConfigManager::save(std::string _filename) {
     // TODO - copy from GlobalVars back to cfg
     if(this->filename.length()==0) {
        if(this->search_paths.size() >0) {
          // TODO: Make this portable
          this->filename = this->search_paths[0] + "/" + _filename;
        } else {
          this->filename = _filename;
        }
     }
     this->cfg.writeFile(this->filename.c_str());
}         

}}};
