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

#include <common.h>
#include <opencraft_entity_data.h>
#include <opencraft_player_data.h>

#include <opencraft_daemon.h>
#include <opencraft_event_dispatcher.h>

#include <python_events.h> // for client_conn - which is basically a python object
#include <string>

opencraft_player_data::opencraft_player_data(boost::python::object *client_conn) {
   this->e_class = ENTITY_CLASS_PLAYER;
   LOG(debug) << "player_data spawned";
}

void opencraft_player_data::tick() {
}

