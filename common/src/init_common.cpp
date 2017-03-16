//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of the OpenCraft launcher.
//
// The OpenCraft client is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// The OpenCraft launcher is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenCraft.  If not, see <http://www.gnu.org/licenses/>.
//
// DESCRIPTION:
//     Handy init and setup tools for use by apps
//
//-----------------------------------------------------------------------------

#include <opencraft/common.h>
#include <opencraft/versiondefs.h>
#include <iostream>
#include <string>

void opencraft::init_common(std::string app_name) {
     std::cout << app_name << " compiled with " << opencraft_framework_longver()  << std::endl;
     std::cout             << "Running on "     << OPENCRAFT_FRAMEWORK_LONG_VER   << std::endl;
     
}
