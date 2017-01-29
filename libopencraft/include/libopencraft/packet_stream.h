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
//     Packet stream: a facility for turning streams of bytes into streams
//     of packets
//
//-----------------------------------------------------------------------------

#pragma once

#include <libopencraft/common.h>
#include <libopencraft/base_packet.h>
#include <libopencraft/raw_packet.h>
#include <deque>

namespace opencraft {
  namespace packets {


// this class is intended to be used as the building block for higher level async goodness
// basically, when data comes in from the socket, pass it to on_recv and get back a vector of packets, which may be 0-length
// note that there's no way to save packets inbetween calls to on_recv
class packet_stream {
   public:
      packet_stream();

      std::vector<raw_packet> on_recv(std::vector<unsigned char> data);
   private:
      std::deque<unsigned char> buf;
      int32_t try_varint();
};

}
}
