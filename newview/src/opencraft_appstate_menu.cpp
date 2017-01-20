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

#include <common.h>
#include <opencraft_appstate_menu.h>
#include <opencraft_video.h>
#include <r_2d.h>

#include <string>


extern opencraft_video *oc_video;
extern void* default_font;

opencraft_appstate_menu::opencraft_appstate_menu() {
    oc_video->enter_2d();
    this->title_gl_tex_id      = load_texture(std::string("gui/title/opencraft.png"));
    this->dirtblock_gl_tex_id  = load_texture(std::string("blocks/dirt.png"));
    this->grassblock_gl_tex_id = load_texture(std::string("blocks/grass_side.png"));

    this->logo_w = LOGO_TEXTURE_WIDTH; // TODO: Make this dynamic - currently defined in common.h
    this->logo_h = LOGO_TEXTURE_HEIGHT;
    this->logo_x = (oc_video->res_w/2) - (this->logo_w/2);
    this->logo_y = 10.0f;

    this->bg_dirt_x = 0.0f;
    this->bg_dirt_y = BLOCK_TEXTURE_SIZE;
    this->bg_dirt_w = oc_video->res_w;
    this->bg_dirt_h = oc_video->res_h-BLOCK_TEXTURE_SIZE;

    this->bg_grass_x = 0.0f;
    this->bg_grass_y = 0.0f;
    this->bg_grass_w = oc_video->res_w;
    this->bg_grass_h = BLOCK_TEXTURE_SIZE;
    
    predraw_text(default_font,255,255,255,(char*)"Join Game",&this->join_game_w,&this->join_game_h,&this->join_game_text_w,&this->join_game_text_h,&this->join_game_text_tex_active);
    predraw_text(default_font,128,128,128,(char*)"Join Game",&this->join_game_w,&this->join_game_h,&this->join_game_text_w,&this->join_game_text_h,&this->join_game_text_tex_inactive);
    this->join_x = (oc_video->res_w/2) - (this->join_game_text_w/2);
    this->join_y = this->logo_y+this->logo_h+10.0f;
    this->join_w = this->join_game_w;
    this->join_h = this->join_game_h;
    this->join_game_text_tex = this->join_game_text_tex_inactive;

    predraw_text(default_font,255,255,255,(char*)"Options",&this->options_tex_w,&this->options_tex_h,&this->options_text_w,&this->options_text_h,&this->options_text_tex_active);
    predraw_text(default_font,128,128,128,(char*)"Options",&this->options_tex_w,&this->options_tex_h,&this->options_text_w,&this->options_text_h,&this->options_text_tex_inactive);
    this->options_x = this->join_x;
    this->options_y = this->join_y+this->join_game_text_h+1.0f;
    this->options_w = this->options_tex_w;
    this->options_h = this->options_tex_h;
    this->options_text_tex = this->options_text_tex_inactive;    

    predraw_text(default_font,255,255,255,(char*)"Quit game",&this->quit_tex_w,&this->quit_tex_h,&this->quit_text_w,&this->quit_text_h,&this->quit_text_tex_active);
    predraw_text(default_font,128,128,128,(char*)"Guit game",&this->quit_tex_w,&this->quit_tex_h,&this->quit_text_w,&this->quit_text_h,&this->quit_text_tex_inactive);
    this->quit_x = this->join_x;
    this->quit_y = this->options_y+this->options_text_h+1.0f;
    this->quit_w = this->quit_tex_w;
    this->quit_h = this->quit_tex_h;
    this->quit_text_tex = this->quit_text_tex_inactive;    

    this->active_menu_item = APPSTATE_MENUITEM_NULL;
}

void opencraft_appstate_menu::update_state(SDL_Event *ev) {
     // first we set the texture inactive
     switch(this->active_menu_item) {
         case APPSTATE_MENUITEM_NULL:
         break;
         case APPSTATE_MENUITEM_JOINGAME:
            this->join_game_text_tex = this->join_game_text_tex_inactive;
         break;
         case APPSTATE_MENUITEM_OPTIONS:
            this->options_text_tex   = this->options_text_tex_inactive;
         break;
         case APPSTATE_MENUITEM_QUITGAME:
            this->quit_text_tex = this->quit_text_tex_inactive;
         break;
     }

     // then we handle movement
     int mouse_x;
     int mouse_y;
     SDL_GetMouseState(&mouse_x,&mouse_y);
     if((mouse_x >= this->join_x) && (mouse_x <= (this->join_x+this->join_w)) && (mouse_y >= this->join_y) && (mouse_y <= (this->join_y+this->join_h))) {
       this->active_menu_item = APPSTATE_MENUITEM_JOINGAME;
     } else if((mouse_x >= this->options_x) && (mouse_x <= (this->options_x+this->options_w)) && (mouse_y >= this->options_y) && (mouse_y <= (this->options_y+this->options_h))) {
       this->active_menu_item = APPSTATE_MENUITEM_OPTIONS;
     } else if((mouse_x >= this->quit_x) && (mouse_x <= (this->quit_x+this->quit_w)) && (mouse_y >= this->quit_y) && (mouse_y <= (this->quit_y+this->quit_h))) {
       this->active_menu_item = APPSTATE_MENUITEM_QUITGAME;
     } else {
       this->active_menu_item = APPSTATE_MENUITEM_NULL;
     }

     // then we set the relevant texture active
     switch(this->active_menu_item) {
         case APPSTATE_MENUITEM_NULL:
         break;
         case APPSTATE_MENUITEM_JOINGAME:
            this->join_game_text_tex = this->join_game_text_tex_active;
         break;
         case APPSTATE_MENUITEM_OPTIONS:
            this->options_text_tex   = this->options_text_tex_active;
         break;
         case APPSTATE_MENUITEM_QUITGAME:
            this->quit_text_tex      = this->quit_text_tex_active;
         break;
     }

     // then we handle other misc events
     switch(ev->type) {
       case SDL_QUIT:
         // TODO: cleanup stuff here or something
       break;
       case SDL_MOUSEBUTTONDOWN:
         this->handle_menuitem(this->active_menu_item);
       break;
     }
}

void opencraft_appstate_menu::handle_menuitem(int menu_item) {
     LOG(debug) << "Handle menu item: " << menu_item;
     switch(menu_item) {
         case APPSTATE_MENUITEM_NULL:
         break;
         case APPSTATE_MENUITEM_JOINGAME:
         break;
         case APPSTATE_MENUITEM_OPTIONS:
         break;
         case APPSTATE_MENUITEM_QUITGAME:
           SDL_Event e;
           e.type = SDL_QUIT;
           SDL_PushEvent(&e);
         break;
     }

}

void opencraft_appstate_menu::render() {
     draw_tiled_quad(this->bg_dirt_x,  this->bg_dirt_y,  this->bg_dirt_w,  this->bg_dirt_h,  BLOCK_TEXTURE_SIZE, BLOCK_TEXTURE_SIZE, this->dirtblock_gl_tex_id);
     draw_tiled_quad(this->bg_grass_x, this->bg_grass_y, this->bg_grass_w, this->bg_grass_h, BLOCK_TEXTURE_SIZE, BLOCK_TEXTURE_SIZE, this->grassblock_gl_tex_id);

     glEnable(GL_BLEND);
     draw_textured_quad(this->logo_x,    this->logo_y,    this->logo_w,    this->logo_h,    this->title_gl_tex_id);
     draw_textured_quad(this->join_x,    this->join_y,    this->join_w,    this->join_h,    this->join_game_text_tex);
     draw_textured_quad(this->options_x, this->options_y, this->options_w, this->options_h, this->options_text_tex);
     draw_textured_quad(this->quit_x,    this->quit_y,    this->quit_w,    this->quit_h,    this->quit_text_tex);
     glDisable(GL_BLEND);
}

