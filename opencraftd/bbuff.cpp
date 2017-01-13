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

bound_buffer::bound_buffer(unsigned char* data, size_t len) {
    _data = (unsigned char*)calloc(len+1024,sizeof(unsigned char)); // always allocate an extra 1kb or so
    _cur_size = len;
    _cursor   = 0;
    memmove((void*)_data, (const void*)data, len);
}

unsigned char* bound_buffer::read(size_t len) {
    // it is up to the caller to free() the return value
    unsigned char* retval = (unsigned char*)calloc(len,sizeof(unsigned char));
    memmove((void*)retval, ((const void*)&(_data[_cursor])), len);
}

void bound_buffer::write(unsigned char* data, size_t len) {
     size_t new_len = _cur_size + len;
     _data = (unsigned char*)realloc((void*)_data,new_len);
     memmove((void*)(&(_data[_cursor])), (const void*)data, len);
}


