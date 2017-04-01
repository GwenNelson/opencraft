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
//     Packet writer class
//
//-----------------------------------------------------------------------------

#include <libopencraft/packet_writer.h>
#include <libopencraft/raw_packet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <vector>
#include <string.h>

namespace opencraft {
  namespace packets {

packet_writer::packet_writer(int _sockfd) {
    this->sockfd     = _sockfd;
}

void packet_writer::write_pack(opencraft_packet *pack) {
     raw_packet rawpack(pack->ident(),pack->pack());
     std::vector<unsigned char> packed = rawpack.pack();

     memset((void*)this->sendbuf,0,sizeof(this->sendbuf));
     
     std::copy(packed.begin(), packed.end(), this->sendbuf);

     send(this->sockfd,this->sendbuf,packed.size(),MSG_NOSIGNAL);
}


}
}
