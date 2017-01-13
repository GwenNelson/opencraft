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

class bound_buffer {
   public:
      bound_buffer();
      bound_buffer(unsigned char* data, size_t len);
      unsigned char* read(size_t len);
      unsigned char* peek(size_t len);
      bound_buffer *read_buf(size_t len);
      int32_t read_varint(int max_bits);
      size_t size();
      void write(unsigned char* data, size_t len);
   private:
      std::vector <unsigned char> _data;
      int            _cursor;
      size_t         _cur_size;
};
