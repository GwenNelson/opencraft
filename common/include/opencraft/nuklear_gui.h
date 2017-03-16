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
//     C++ wrapper and app framework for nuklear
//     Based heavily on the demo code that comes with Nuklear
//
//-----------------------------------------------------------------------------

#include <opencraft/common.h>

#include <opencraft/nuklear.h>
#include <opencraft/nuklear_sdl_gl3.h>

#include <string>

namespace opencraft { namespace nuklear {
  class App {
     public:
       App(std::string app_name);
  };
}}
