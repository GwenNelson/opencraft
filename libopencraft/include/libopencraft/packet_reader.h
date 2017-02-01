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
//     Packet reader class
//
//-----------------------------------------------------------------------------

#pragma once

#include <libopencraft/base_packet.h>

namespace opencraft {
  namespace packets {

class packet_reader {
   public:
      packet_reader(int _sockfd, int _proto_mode, bool _is_client);
      opencraft_packet* read_pack();
      int read_varint();
   private:
      int  sockfd;
      int  proto_mode;
      bool is_client;
      unsigned char recvbuf[4096];
};

}
}
