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

#include <libopencraft/common.h>
#include <libopencraft/packet_stream.h>
#include <libopencraft/raw_packet.h>
#include <libopencraft/base_packet.h>
#include <vector>

namespace opencraft {
  namespace packets {

packet_stream::packet_stream() {
}

int32_t packet_stream::try_varint() {
    int retval=0;
    if(this->buf.size()==0) return 0;
    unsigned int i=0;
    for(i=0; i < 5; ++i) {
        if(i > this->buf.size()) return 0;
         retval |= (this->buf[i] & 0x7F) << (i*7);
         if(!(this->buf[i] & 0x80)) {
            break;
         }
     }
     return retval;
}

std::vector<raw_packet> packet_stream::on_recv(std::vector<unsigned char> data) {
    std::vector<raw_packet> retval = std::vector<raw_packet>();

    for(int i=0; i< data.size(); i++) {
        this->buf.push_back(data[i]);
    }

    int32_t packlen = this->try_varint();
    while(packlen > 0) {
       packlen = this->try_varint();
       if(packlen >0) {
          if(this->buf.size() > packlen) {
             std::vector<unsigned char> tmpbuf = std::vector<unsigned char>();
             int packlen_len = varint_size(packlen);
             for(int i=0; i< packlen_len; i++) {
                 tmpbuf.push_back(this->buf.front());
                 this->buf.pop_front();
             }
             for(int i=0; i< packlen; i++) {
                 tmpbuf.push_back(this->buf.front());
                 this->buf.pop_front();
             }
             retval.push_back(raw_packet(tmpbuf));
          }
       }
    }
    this->buf.clear();
    return retval;
}

}
}
