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
//     Simple dummy client plugin
//
//-----------------------------------------------------------------------------

#include <plugin_api.h>
#include <unistd.h>


// it is recommended to include the compatible versions as a static array and then add in the incompatible versions in dummy_update_versions
// the is_installed field can be updated by the update_version_data() API function
// this dummy plugin demonstrates 2 different clients: one free and one nonfree
client_version_info_t static_versions[] = {
    {.is_free            = true,
     .is_compatible      = true,
     .is_installed       = false,
     .client_name        = "Dummy free software client",
     .client_license     = "GPLv2",
     .version_id         = "1.0",
     .source_repo_url    = "http://github.com/GarethNelson/ThisRepoDoesNotExist",
     .source_repo_branch = "1.0",
     .protocol_versions  = "47,107"
    },
    {.is_free            = true,
     .is_compatible      = true,
     .is_installed       = false,
     .client_name        = "Dummy free software client",
     .client_license     = "GPLv2",
     .version_id         = "2.0",
     .source_repo_url    = "http://github.com/GarethNelson/ThisRepoDoesNotExist",
     .source_repo_branch = "2.0",
     .protocol_versions  = "47,107,315"
    },
    {.is_free            = false,
     .is_compatible      = true,
     .is_installed       = false,
     .client_name        = "Dummy proprietary client",
     .client_license     = "",
     .version_id         = "1.8",
     .source_repo_url    = NULL,
     .source_repo_branch = NULL,
     .protocol_versions  = "47"
    }
};

// to test update_versions, here we define an update to our dummy free client that's compatible, and a nonfree one that isn't
// dummy_update_versions() then merges the 2
client_version_info_t new_versions[] = {
    {.is_free            = true,
     .is_compatible      = true,
     .is_installed       = false,
     .client_name        = "Dummy free software client",
     .client_license     = "GPLv2",
     .version_id         = "2.1",
     .source_repo_url    = "http://github.com/GarethNelson/ThisRepoDoesNotExist",
     .source_repo_branch = "2.1",
     .protocol_versions  = "47,107,315"
    },
    {.is_free            = false,
     .is_compatible      = false,
     .is_installed       = false,
     .client_name        = "Dummy proprietary client",
     .client_license     = "",
     .version_id         = "1.11.2",
     .source_repo_url    = NULL,
     .source_repo_branch = NULL,
     .protocol_versions  = "316"
    }
};

// we keep track of available versions in an array of pointers, last entry is NULL
// before init_module() is run this is null, init_module() sets it up
client_version_info_t **available_versions = NULL;

// we also keep track of available+supported versions in the same format
client_version_info_t **supported_versions = NULL;

void dummy_update_versions() {
     // in a real plugin, we'd talk to the server and get updated version info here
     // instead, we'll simulate the lag using sleep()
     LOG(info) << "dummy_client_plugin: " << sizeof(static_versions)/sizeof(client_version_info_t) << " currently known versions";
     LOG(info) << "dummy_client_plugin: pretending to download latest version data...";
     sleep(1);
     // when downloading version data, we should get ALL available versions, including those we already know about
     LOG(info) << "dummy_client_plugin: downloaded version data for " << (sizeof(static_versions)+sizeof(new_versions))/sizeof(client_version_info_t) << " versions";

     // here we actually update the available_versions array, as a quick hack we just free() it and recreate it
     // this is VERY hacky, a real plugin should instead do some sort of merge with the new data
     // a real plugin should probably also use std::vector internally
     free((void*)available_versions);
     int static_version_count = sizeof(static_versions) / sizeof(client_version_info_t);
     int new_version_count    = sizeof(new_versions)    / sizeof(client_version_info_t);

     available_versions = (client_version_info_t**)malloc(sizeof(client_version_info_t*)*(static_version_count+new_version_count+1));

     // we also update the supported versions

     free((void*)supported_versions);
     supported_versions = (client_version_info_t**)malloc(sizeof(client_version_info_t*)*(static_version_count+new_version_count+1)); // yeah, i know, wasteful

     int i   = 0;
     int s_i = 0;

     // first shove the static versions into place
     for(i=0; i<static_version_count; i++) {
         LOG(debug) << "dummy_client_plugin: Adding version update: " << static_versions[i].client_name << ", version " << static_versions[i].version_id;
         available_versions[i] = &(static_versions[i]);
         if(static_versions[i].is_compatible) {
            supported_versions[s_i] = &(static_versions[i]);
            s_i++;
         }
     }
     
     // and now the new versions
     int n_i=0;
     for(n_i=0; i<(new_version_count+static_version_count); i++) {
         n_i = i-static_version_count;
         LOG(debug) << "dummy_client_plugin: Adding version update: " << new_versions[n_i].client_name << ", version " << new_versions[n_i].version_id;
         available_versions[i] = &(new_versions[n_i]);
         if(new_versions[n_i].is_compatible) {
            supported_versions[s_i] = &(new_versions[n_i]);
            s_i++;
         }
     }
     available_versions[i] = NULL;
     supported_versions[i] = NULL;
}

client_version_info_t **dummy_get_avail_versions() {
     // this function returns an array that should NOT be modified by the caller
     return available_versions;
}

client_version_info_t **dummy_get_supported_versions() {
     return supported_versions;
}

void dummy_init() {
     // this will be called as soon as the plugin is loaded and is where we should setup any tables and structs
     // in this dummy, we update available_versions with static_versions

     int static_version_count    = sizeof(static_versions) / sizeof(client_version_info_t);
     int supported_version_count = 0; // updated in loop below

     // allocate memory for all available versions
     available_versions = (client_version_info_t**)malloc(sizeof(client_version_info_t*)*(static_version_count+1)); // we allocate 1 extra for the terminating NULL

     // we also allocate memory for the supported versions, which we can resize later if needed
     supported_versions = (client_version_info_t**)malloc(sizeof(client_version_info_t*)*(static_version_count+1));

     // normally a plugin should only output errors to the log in this function, but it helps with debugging to see some info here
     int i=0;
     int s_i=0; // supported versions iterator
     for(i=0; i<static_version_count; i++) {
         LOG(debug) << "dummy_client_plugin: Adding static version: " << static_versions[i].client_name << ", version " << static_versions[i].version_id;
         available_versions[i] = &(static_versions[i]);
         if(static_versions[i].is_compatible) {
            supported_versions[s_i] = &(static_versions[i]);
            s_i++;
         }
     }
     supported_versions[s_i] = NULL;
     available_versions[i]   = NULL;

}

bool dummy_update() {
     return false; // we're always gonna be the latest version
}

module_info_t module_info = {
  .module_type      = MODTYPE_CLIENT,
  .module_name      = "Dummy client module",
  .module_author    = "Gareth Nelson",
  .module_copyright = "Copyright (C) 2017 Gareth Nelson",
  .module_license   = "GPLv2",
  .module_version   = "0.1",
  .init_module      = dummy_init,
  .check_for_update = dummy_update
};

client_api_t client_api = {
  .update_version_data    = dummy_update_versions,
  .get_available_versions = dummy_get_avail_versions,
  .get_supported_versions = dummy_get_supported_versions
};
