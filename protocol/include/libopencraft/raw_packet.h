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

#pragma once

#include <libopencraft/base_packet.h>

namespace opencraft {
  namespace packets {

class raw_packet : public opencraft_packet {
   public:
      raw_packet(std::vector<unsigned char> data);
      raw_packet(int32_t length, int32_t ident, std::vector<unsigned char> data);
      raw_packet(int32_t ident, std::vector<unsigned char> data);

      int32_t pack_length;
      int32_t pack_ident;
      std::vector<unsigned char> pack_data;

};

}
}
