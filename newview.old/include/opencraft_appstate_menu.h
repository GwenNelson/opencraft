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
//     Menu appstate
//
//-----------------------------------------------------------------------------

#pragma once

#include <common.h>
#include <SDL.h>
#include <opencraft_appstate.h>

#define APPSTATE_MENUITEM_NULL     0
#define APPSTATE_MENUITEM_JOINGAME 1
#define APPSTATE_MENUITEM_OPTIONS  2
#define APPSTATE_MENUITEM_QUITGAME 3

class opencraft_appstate_menu : public opencraft_appstate{
   public:
     opencraft_appstate_menu();
     ~opencraft_appstate_menu();
     void update_state(SDL_Event *ev);
     void render();
     void handle_menuitem(int menu_item);
   private:
     unsigned int title_gl_tex_id;
     unsigned int dirtblock_gl_tex_id;
     unsigned int grassblock_gl_tex_id;

     unsigned int join_game_text_tex;
     unsigned int join_game_text_tex_active;
     unsigned int join_game_text_tex_inactive;
     int join_game_w;
     int join_game_h;
     int join_game_text_w;
     int join_game_text_h;

     unsigned int options_text_tex;
     unsigned int options_text_tex_active;
     unsigned int options_text_tex_inactive;
     int options_tex_w;
     int options_tex_h;
     int options_text_w;
     int options_text_h;

     unsigned int quit_text_tex;
     unsigned int quit_text_tex_active;
     unsigned int quit_text_tex_inactive;
     int quit_tex_w;
     int quit_tex_h;
     int quit_text_w;
     int quit_text_h;

     int active_menu_item;

     float join_x;
     float join_y;
     float join_w;
     float join_h;

     float options_x;
     float options_y;
     float options_w;
     float options_h;

     float quit_x;
     float quit_y;
     float quit_w;
     float quit_h;

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
