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
//     Raw packet class
//
//-----------------------------------------------------------------------------

#include <base_packet.h>
#include <raw_packet.h>

namespace opencraft {
  namespace packets {

raw_packet::raw_packet(std::vector<unsigned char> data) {
  this->packed = data;
  this->bufpos = 0;
  this->pack_length = this->unpack_varint();
  this->pack_ident  = this->unpack_varint();
  this->pack_data   = this->unpack_bytes();
}

raw_packet::raw_packet(int32_t length, int32_t ident, std::vector<unsigned char> data) {
  this->bufpos      = 0;
  this->packed      = std::vector<unsigned char>();
  this->pack_length = length;
  this->pack_ident  = ident;
  this->pack_data   = data;
  this->pack_varint(length);
  this->pack_varint(ident);
  this->pack_bytes(data);
}

raw_packet::raw_packet(int32_t ident, std::vector<unsigned char> data) {
  this->bufpos        = 0;
  this->packed        = std::vector<unsigned char>();
  this->pack_ident    = ident;
  this->pack_data     = data;
  int pack_ident_size = varint_size(ident);
  this->pack_length   = pack_ident_size + data.size();
  this->pack_varint(this->pack_length);
  this->pack_varint(this->pack_ident);
  this->pack_bytes(this->pack_data);
}

}
}
