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
//     A basic client
//
//-----------------------------------------------------------------------------

#pragma once

#include <libopencraft/common.h>
#include <libopencraft/base_packet.h>
#include <libopencraft/packet_stream.h>

#include <string>
#include <map>
#include <vector>
#include <tuple>

namespace opencraft {
  namespace client {

typedef void (*pack_callback_t)(void* ctx, opencraft::packets::opencraft_packet *pack); // callbacks should NEVER delete/free the pack param lest bad things happen

class basic_client {
   public:

      void register_handler(int32_t pack_ident, int32_t proto_mode, pack_callback_t cb, void* ctx);

      void on_recv(std::vector<unsigned char> data); // call this when data is available from the socket
      std::vector<unsigned char> on_send();          // call this to get data that should be transmitted to the server

      void send_hs(std::string hostname, int port, int new_proto_mode);
      void send_pack(opencraft::packets::opencraft_packet *pack); // call this to queue a packet for transmission on the next on_send()
      
      bool compression_enabled;
      void set_compression(int32_t new_threshold);
   protected:
      int proto_mode;

   private:
      std::map<int32_t, std::map<int32_t,std::vector<std::tuple<void*, pack_callback_t> > > > pack_callbacks;
      std::vector<unsigned char> sendbuf;
      opencraft::packets::packet_stream p_stream;
      int32_t compression_threshold;
};

}
}
