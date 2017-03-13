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

module_info_t module_info = {
  .module_type      = MODTYPE_CLIENT,
  .module_name      = "Dummy client module",
  .module_author    = "Gareth Nelson",
  .module_copyright = "Copyright (C) 2017 Gareth Nelson",
  .module_license   = "GPLv2"
};
