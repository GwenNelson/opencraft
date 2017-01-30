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

#include <libopencraft/common.h>
#include <libopencraft/spawning_client.h>
#include <libopencraft/proto_constants.h>
#include <libopencraft/packets.autogen.h>
#include <string>
#include <map>
#include <vector>

namespace opencraft {
  namespace client {

void spawning_client_login_cb(void* ctx, opencraft::packets::login_success_login_downstream *pack) {
     spawning_client* client = (spawning_client*)ctx;
     client->login_cb(pack);
}

void spawning_client_compression_cb(void* ctx, opencraft::packets::login_set_compression_login_downstream *pack) {
     spawning_client* client = (spawning_client*)ctx;
     client->set_compression(pack->a);
}

spawning_client::spawning_client(std::string username) {
    this->send_hs("127.0.0.1",25565,OPENCRAFT_STATE_LOGIN);
    opencraft::packets::login_start_login_upstream login_start_pack(username);
    this->register_handler(OPENCRAFT_PACKIDENT_LOGIN_SUCCESS_LOGIN_DOWNSTREAM,spawning_client_login_cb,(void*)this);
    this->register_handler(OPENCRAFT_PACKIDENT_LOGIN_SET_COMPRESSION_LOGIN_DOWNSTREAM,spawning_client_compression_cb,(void*)this);
    this->send_pack(&login_start_pack);
}

void spawning_client::login_cb(opencraft::packets::login_success_login_downstream *pack) {
     this->proto_mode   = OPENCRAFT_STATE_PLAY;
     this->avatar_uuid  = pack->a;
     this->display_name = pack->b;
}


}
}
