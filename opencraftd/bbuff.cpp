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

#include <common.h>
#include <bbuff.h>
#include <string.h>
#include <utils.h>

#include <vector>

bound_buffer::bound_buffer() {
    _data.resize(0);
    _cursor   = 0;
}

bound_buffer::bound_buffer(unsigned char* data, size_t len) {
    _data.resize(len);
    _cursor   = 0;
    write(data,len);
}

unsigned char* bound_buffer::peek(size_t len) {
    return &(_data[_cursor]);
}

unsigned char* bound_buffer::read(size_t len) {
    unsigned char* retval = peek(len);
    _cursor += len;
    return retval;
}

size_t bound_buffer::size() {
    return _data.size();
}

int32_t bound_buffer::read_varint(int max_bits) {
    unsigned char *intbuf = peek(max_bits / 8);
    int32_t        retval = parse_var_int(intbuf, max_bits/8);
    _cursor += varint_size(retval);
    return retval;
}

void bound_buffer::write(unsigned char* data, size_t len) {
     size_t new_len = _cur_size + len;

     for(int i=0; i<len; i++) {
       _data.push_back(data[i]);
     }
     _data.resize(new_len);
}


