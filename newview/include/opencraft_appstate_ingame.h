//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of the OpenCraft client.
//
// The OpenCraft client is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// The OpenCraft client is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenCraft.  If not, see <http://www.gnu.org/licenses/>.
//
// DESCRIPTION:
//     ingame appstate
//
//-----------------------------------------------------------------------------

#include <common.h>
#include <SDL.h>
#include <opencraft_appstate.h>
#include <opencraft_connection.h>

#include <string>
#include <vector>
#include <tuple>
#include <map>

#include <libopencraft/packets.autogen.h>
#include <libopencraft/version.h>
#include <libopencraft/proto_constants.h>

// loading = loading assets, connecting = duh, playing = duh, dead = dead and awaiting respawn

#define INGAME_LOADING    0
#define INGAME_CONNECTING 1
#define INGAME_PLAYING    2
#define INGAME_DEAD       3

// SOCK_CONN is opening the socket, HS is handshake, rest are obvious

#define INGAME_CONNECTING_SOCK_CONN        4
#define INGAME_CONNECTING_HS               5
#define INGAME_CONNECTING_LOGIN_SUCC       6
#define INGAME_CONNECTING_LOGIN_FAIL       7
#define INGAME_CONNECTING_DOWNLOAD_TERRAIN 8

typedef std::tuple<std::string, unsigned int> pending_blockload_t; // texture path, block ID
typedef std::vector<pending_blockload_t> pending_blockloads_t;

class opencraft_appstate_ingame : public opencraft_appstate{
   public:
     opencraft_appstate_ingame(std::string server_addr);
     ~opencraft_appstate_ingame();
     void update_state(SDL_Event *ev);
     void update_loading();
     void render();
     int  cur_state;  // overall state: loading/connecting/play/dead etc
     int  conn_state; // substate for connecting state 
 
     double  progress; // for loading and connecting, meaning should be obvious
     double  total;

     opencraft_connection *client_conn;
   private:
     void load_stuff();

     void update_loading(SDL_Event *ev);
     void update_connecting(SDL_Event *ev);
     void update_playing(SDL_Event *ev);
     void update_dead(SDL_Event *ev);
     void render_loading();
     void render_connecting();
     void render_playing();
     void render_dead();
     std::string _server_addr;
     void add_blockload(std::string texture_path, unsigned int tex_id); // add a block that needs loading
     void add_blocks(); // call add_blockload for all the blocks
     std::map<unsigned int,unsigned int> block_tex_id;

     pending_blockloads_t pending_loads;

     unsigned int loading_tex_id;
     float        loading_x;
     float        loading_y;
     int        loading_w;
     int        loading_h;
     int        loading_text_w;
     int        loading_text_h;

     unsigned int connecting_tex_id;
     float        connecting_x;
     float        connecting_y;
     int        connecting_w;
     int        connecting_h;
     int        connecting_text_w;
     int        connecting_text_h;

     unsigned int dirtblock_gl_tex_id;
     unsigned int grassblock_gl_tex_id;
     unsigned int progress_gl_tex_id;
     double progress_w;


     float bg_dirt_x;
     float bg_dirt_y;
     float bg_dirt_w;
     float bg_dirt_h;

     float bg_grass_x;
     float bg_grass_y;
     float bg_grass_w;
     float bg_grass_h;     

};
