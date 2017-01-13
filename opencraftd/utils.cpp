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
//     Misc utility functions
//
//-----------------------------------------------------------------------------

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

#include <common.h>

int varint_size(int32_t input) {
	for (int32_t x = 1; x < 5; x++) {
		if ((input & -1 << x * 7) == 0) return x;
	}
	return 5;
}

int32_t parse_var_int(unsigned char* buf, size_t buflen) {
        int32_t retval=0;
        int v2        =0;
        signed char v3;
        do {
                if (v2 >= buflen) return 0;
                v3 = buf[v2];
                retval |= (v3 & 127) << (v2++ * 7);
                if (v2 > 5) return retval;
        } while ((v3 & 128) == 128);
        return retval;
}

void hexdump(unsigned char *buffer, unsigned long index, unsigned long width) {
  unsigned long i;
  for (i=0;i<index;i++)
	{
	printf("%02x ",buffer[i]);
	}
  for (unsigned long spacer=index;spacer<width;spacer++)
	printf("	");
  printf(": ");
  for (i=0;i<index;i++)
	{
	if (buffer[i] < 32) printf(".");
	else printf("%c",buffer[i]);
	}
  printf("\n");
}
