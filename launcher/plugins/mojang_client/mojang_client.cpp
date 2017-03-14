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
//     Plugin for the official mojang client
//
//-----------------------------------------------------------------------------

#include <plugin_api.h>
#include <unistd.h>

#define MOJANG_CLIENT_NAME        "Official Mojang Client"
#define MOJANG_VERSIONS_MANIFEST  "https://launchermeta.mojang.com/mc/game/version_manifest.json"

// the binary repo URL here is a JSON file used to get the URLs of the various .jar files etc
client_version_info_t static_versions[] = {
    {.is_free            = false,
     .is_compatible      = true,
     .is_installed       = false,
     .client_name        = MOJANG_CLIENT_NAME,
     .client_license     = "",
     .version_id         = "1.11",
     .binary_repo_url    = "https://launchermeta.mojang.com/mc/game/7a3e41e164f3e3124c05a8bc782bba466d20c431/1.11.json",
     .source_repo_url    = NULL,
     .source_repo_branch = NULL,
     .protocol_versions  = "315"
    },
};

// only supported versions are the static versions, for now
client_version_info_t **supported_versions = NULL;
client_version_info_t **available_versions = NULL;

void mojang_update_versions() {
     LOG(info) << "mojang_client_plugin: update versions not yet implemented!";

     // TODO: implement this by parsing the manifest file

}

client_version_info_t **mojang_get_avail_versions() {
     return available_versions;
}

client_version_info_t **mojang_get_supported_versions() {
     return supported_versions;
}

void mojang_init() {
     int static_version_count    = sizeof(static_versions) / sizeof(client_version_info_t);
     available_versions = (client_version_info_t**)malloc(sizeof(client_version_info_t*)*(static_version_count+1));
     supported_versions = (client_version_info_t**)malloc(sizeof(client_version_info_t*)*(static_version_count+1));

     int i=0;
     for(i=0; i<static_version_count; i++) {
         available_versions[i] = &(static_versions[i]);
         supported_versions[i] = &(static_versions[i]);
     }
     available_versions[i] = NULL;
     supported_versions[i] = NULL;
}

bool mojang_update() {
     return false; // we're always gonna be the latest version - at least for now
}

module_info_t module_info = {
  .module_type      = MODTYPE_CLIENT,
  .module_name      = "Mojang minecraft client module",
  .module_author    = "Gareth Nelson",
  .module_copyright = "OpenCraft client module Copyright (C) 2017 Gareth Nelson,\n Mojang is a registered trademark of Mojang Synergies AB",
  .module_license   = "GPLv2",
  .module_version   = "0.1",
  .init_module      = mojang_init,
  .check_for_update = mojang_update
};

client_api_t client_api = {
  .update_version_data    = mojang_update_versions,
  .get_available_versions = mojang_get_avail_versions,
  .get_supported_versions = mojang_get_supported_versions
};
