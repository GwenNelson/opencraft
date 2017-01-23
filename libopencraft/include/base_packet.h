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
//     Base packet class
//
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <tuple>
#include <vector>

namespace opencraft {
  namespace packets {


// Packet classes should be considered as read only once created
class opencraft_packet {
   public:
      std::vector<unsigned char> pack(); // pack into a transmission-ready format, no compression or encryption yet applied but length prefix in place
      void unpack(std::vector<unsigned char> packdata); // used by the static method to do unpacking on each packet
      static opencraft_packet *unpack_packet(std::vector<unsigned char> packdata); // unpack raw data into a packet
      std::string name(); // return the packet name

      std::string unpack_string(); // fuck you unicode

      bool                           unpack_boolean();
      unsigned char                  unpack_byte();
      char                           unpack_char();
      std::string                    unpack_string16();
      double                         unpack_double();
      float                          unpack_float();
      int32_t                        unpack_int();
      int64_t                        unpack_long();
      int16_t                        unpack_short();
      std::string                    unpack_string8();
      int32_t                        unpack_varint();
      std::tuple<float,float,float>  unpack_position();
      int32_t                        unpack_enum();
      std::vector<unsigned char>     unpack_bytes();

      std::vector<unsigned char> packed; // buffer used for packing - shou
      int bufpos; // current offset into the buffer for read/write operations, should be set to 0 in constructor
};

}
}
