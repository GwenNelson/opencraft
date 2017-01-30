//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of OpenCraft.
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
//     A client implementation that connects and spawns an avatar
//
//-----------------------------------------------------------------------------

#pragma once

#include <libopencraft/common.h>
#include <libopencraft/basic_client.h>
#include <libopencraft/packets.autogen.h>

#include <string>
#include <map>
#include <vector>

namespace opencraft {
  namespace client {

class spawning_client: public basic_client {
   public:
      spawning_client(std::string username);
      void login_cb(opencraft::packets::login_success_login_downstream *pack);
      std::string avatar_uuid;
      std::string display_name;
   private:
};

}
}
