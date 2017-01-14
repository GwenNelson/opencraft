"""
OpenCraft: gen_events_header.py - generate a header file full of defines for event types
Copyright (C) 2016 GarethNelson

This file is part of OpenCraft

OpenCraft is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

OpenCraft is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OpenCraft.  If not, see <http://www.gnu.org/licenses/>.

This is used by the build to create a header file full of event types.
If for some mad reason you need to use it directly, just redirect stdout to the relevant place.
"""

from event_types import *

print '#pragma once'

print '#define DEFAULT_PROTO_VER %s' % str(DEFAULT_PROTO)

for k,v in event_ids.items():
    print '#define EVENT_%s %s' % (k.upper(),hex(v))
