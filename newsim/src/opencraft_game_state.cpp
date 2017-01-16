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
//     Game state
//
//-----------------------------------------------------------------------------


#include <common.h>

#include <event_types.autogen.h>
#include <opencraft_daemon.h>
#include <opencraft_event_dispatcher.h>

#include <opencraft_game_state.h>
#include <opencraft_entity_data.h>
#include <opencraft_player_data.h>

#include <boost/python.hpp>
#include <frameobject.h>

#include <stdlib.h>

extern opencraft_daemon* oc_daemon;

void gamestate_connect_cb(int32_t event_id, void* event_data) {
     event_data_onconnect_t *e_data              = (event_data_onconnect_t*)event_data;
     int32_t new_ent_id                          = (int32_t)random();
     oc_daemon->game_state->entities[new_ent_id] = new opencraft_player_data(e_data->client_conn);
}

opencraft_game_state::opencraft_game_state() {
    oc_daemon->event_dispatcher->register_handler(EVENT_ON_CONNECT,gamestate_connect_cb);
}

void opencraft_game_state::tick() {
}

