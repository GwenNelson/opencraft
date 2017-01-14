"""
OpenCraft: event_types.py - the canonical source of event types
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

This module is the canonical source of event types and is used both in the python code and to autogenerate a header file for the C++ code.
Event types are integers stored in event_ids - it's recommended to only do a single lookup at init time
"""

from quarry.data import packets

DEFAULT_PROTO    = 315 # update this if the protocol changes

EVENT_ID = 0 # we increment this each time an event is added, making the event IDs unique

event_ids      = {}
event_names    = {}
packets_events = {} # maps packet IDs to event IDs that should be used for that particular packet

def add_event(event_name):
    global event_ids
    global event_names
    global EVENT_ID
    event_ids[event_name] = EVENT_ID
    event_names[EVENT_ID] = event_name
    EVENT_ID += 1
    return EVENT_ID-1

# the server sends on_packet_ events for every packet
# on_packet_ events have these params: (client_conn, packet, packet_len)
# data types depend which side we're on, on the python side the "packet" param is a bound buffer and packet_len isn't there
# on the C++ side packet is an unsigned char* and packet_len is a size_t
for k,v in packets.packet_names.items():
    if k[0]==DEFAULT_PROTO:
       packets_events[k[3]] = add_event('on_packet_%s' % v)

# these events are used in various places, params are listed next to them
add_event('on_connect')      # (new_client_conn) sent by server class
add_event('on_player_moved') # (player_data) sent by player_data class - should look into extending this to give the old location/rotation data

