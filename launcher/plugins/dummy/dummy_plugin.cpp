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
//     Simple dummy plugin
//
//-----------------------------------------------------------------------------

#include <plugin_api.h>

void dummy_init() {
     // this will be called as soon as the plugin is loaded
     // normally plugins should NOT spit anything out to the logger other than errors, but this is just an example
     LOG(info) << "Hello from dummy_plugin";
}

bool dummy_update() {
     return false; // we're always gonna be the latest version
}

void dummy_update_versions() {
     LOG(debug) << "dummy_plugin is updating version info, w00t";
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
  .update_version_data = dummy_update_versions
};
