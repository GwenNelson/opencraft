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
//     Join game appstate - NOT the same thing as being ingame
//
//-----------------------------------------------------------------------------

#pragma once

#include <common.h>
#include <SDL.h>
#include <opencraft_appstate.h>

#include <tuple>
#include <vector>

typedef std::tuple<std::string, std::string, unsigned int, unsigned int, float, float> joingame_entry_t; // name, address, name_tex_id, addr_tex_id, w, h
typedef std::vector<joingame_entry_t> joingame_entries_t;

class opencraft_appstate_joingame : public opencraft_appstate{
   public:
     opencraft_appstate_joingame();
     ~opencraft_appstate_joingame();
     void update_state(SDL_Event *ev);
     void render();
     void add_entries(); //TODO: Make this load from a config file or something
     void add_entry(std::string name, std::string address);
   private:
     joingame_entries_t entries;
     unsigned int title_gl_tex_id;
     unsigned int dirtblock_gl_tex_id;
     unsigned int grassblock_gl_tex_id;

     float logo_x;
     float logo_y;
     float logo_w;
     float logo_h;

     float bg_dirt_x;
     float bg_dirt_y;
     float bg_dirt_w;
     float bg_dirt_h;

     float bg_grass_x;
     float bg_grass_y;
     float bg_grass_w;
     float bg_grass_h;     

};
