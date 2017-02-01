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

#include <libopencraft/packet_reader.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <vector>

namespace opencraft {
  namespace packets {

packet_reader::packet_reader(int _sockfd, int _proto_mode, bool _is_client) {
    this->sockfd     = _sockfd;
    this->proto_mode = _proto_mode;
    this->is_client  = _is_client;
}

opencraft_packet* packet_reader::read_pack() {
    int packsize = this->read_varint();
    recv(this->sockfd,(void*)this->recvbuf,packsize,0);
    std::vector<unsigned char> tmpbuf;
    tmpbuf.assign(this->recvbuf, this->recvbuf+packsize);
    return opencraft_packet::unpack_packet(this->proto_mode,this->is_client,tmpbuf);
}

int packet_reader::read_varint() {
    int retval=0;
    unsigned char buf;
    for(int i=0; i < 5; ++i) {
        if(recv(this->sockfd,&buf,1,0)!=1) return retval;
        retval |= (buf & 0x7F) << (i*7);
        if(!(buf & 0x80)) {
           break;
        }
    }
    return retval;

}

}
}
