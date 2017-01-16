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
//     Player data - username, position, etc etc
//
//-----------------------------------------------------------------------------

#pragma once

#include <common.h>

#include <event_types.autogen.h>
#include <opencraft_daemon.h>
#include <opencraft_event_dispatcher.h>

#include <python_events.h> // for client_conn - which is basically a python object
#include <string>

/*
 This class is basically spawned by the game_state class for each player
 */
class opencraft_player_data {
   public:
     opencraft_player_data(object *client_conn);
     std::string player_username;
     // TODO add position, UUID, entity ID and all that stuff
     void tick();
};
