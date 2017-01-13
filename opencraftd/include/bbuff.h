//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of the OpenCraft server.
//
// The OpenCraft server is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// The OpenCraft server is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenCraft.  If not, see <http://www.gnu.org/licenses/>.
//
// DESCRIPTION:
//      Port of bound buffer from spockbot
//
//-----------------------------------------------------------------------------

#pragma once

#include <unistd.h>
#include <vector>
#include <string>

class bound_buffer {
   public:
      bound_buffer();
      bound_buffer(unsigned char* data, uint32_t len);
      unsigned char* read(uint32_t len);
      unsigned char* peek(uint32_t len);
      bound_buffer *read_buf(uint32_t len);
      int32_t read_varint(int max_bits);
      std::string read_string();
      uint32_t size();
      void write(unsigned char* data, uint32_t len);
      void clear_backlog();
   private:
      std::vector <unsigned char> _data;
      unsigned int     _cursor;
      uint32_t         _cur_size;
};
