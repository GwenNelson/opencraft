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

namespace opencraft {
  namespace packets {

class opencraft_packet {
   public:
      unsigned char* pack(); // pack into a transmission-ready format, no compression or encryption yet applied but length prefix in place
      static opencraft_packet unpack_packet(unsigned char* packdata); // unpack raw data into a packet
      std::string name(); // return the packet name
   private:
   // subclasses should include a tuple of the packet contents here named pack_fields
   // the constructor should setup the packet fields
   // the name() method should be implemented and return the name of the packet
};

}
}
