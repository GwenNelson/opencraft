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
#include <opencraft_appstate_ingame.h>
#include <opencraft_video.h>
#include <r_2d.h>

extern opencraft_video *oc_video;
extern void* default_font;

opencraft_appstate_ingame::opencraft_appstate_ingame(std::string server_addr) {
    this->cur_state    = INGAME_LOADING;
    this->total        = 0.0;
    this->progress     = 0.0;
    this->_server_addr = server_addr;

    oc_video->enter_2d(); // must do this again if we go back from actual play
    this->load_stuff(); // stuff that must be loaded before we can render even the loading page
    this->add_blocks();
}

void opencraft_appstate_ingame::load_stuff() {
     LOG(debug) << "load_stuff() for ingame";

     unsigned int tex_id;
     int w;
     int h;
     int text_w;
     int text_h;
     predraw_text(default_font,255,255,255,(char*)"Loading stuff",&w,&h,&text_w,&text_h,&tex_id);

     this->loading_w      = w;
     this->loading_h      = h;
     this->loading_text_w = text_w;
     this->loading_text_h = text_h;
     this->loading_tex_id = tex_id;
     this->loading_x = (oc_video->res_w/2) - (this->loading_text_w/2);
     this->loading_y = (oc_video->res_h/2) - (this->loading_text_h/2);


     this->dirtblock_gl_tex_id  = load_texture(std::string("blocks/dirt.png"));
     this->grassblock_gl_tex_id = load_texture(std::string("blocks/grass_side.png"));

     this->bg_dirt_x = 0.0f;
     this->bg_dirt_y = BLOCK_TEXTURE_SIZE;
     this->bg_dirt_w = oc_video->res_w;
     this->bg_dirt_h = oc_video->res_h-BLOCK_TEXTURE_SIZE;

     this->bg_grass_x = 0.0f;
     this->bg_grass_y = 0.0f;
     this->bg_grass_w = oc_video->res_w;
     this->bg_grass_h = BLOCK_TEXTURE_SIZE;

     this->progress_gl_tex_id = load_texture("share/progress_pixel.png");
}

void opencraft_appstate_ingame::update_state(SDL_Event *ev) {
     switch(this->cur_state) {
        case INGAME_LOADING: {
             this->update_loading(ev);
        break;}
        case INGAME_CONNECTING: {
             this->update_connecting(ev);
        break;}
        case INGAME_PLAYING: {
             this->update_playing(ev);
        break;}
        case INGAME_DEAD: {
             this->update_dead(ev);
        break;}
     }
}

void opencraft_appstate_ingame::add_blockload(std::string texture_path, unsigned int tex_id) {
     pending_blockload_t new_blockload(texture_path, tex_id);
     this->pending_loads.push_back(new_blockload);
}

void opencraft_appstate_ingame::add_blocks() {
     #include "loadblocks.inc"
     this->total += (this->pending_loads.size());
}

void opencraft_appstate_ingame::update_loading(SDL_Event *ev) {
     if(this->progress >= this->total) return;
     pending_blockload_t curblockload = this->pending_loads.back();
     this->pending_loads.pop_back();
     
     std::string tex_path     = std::get<0>(curblockload);
     unsigned int curblockid  = std::get<1>(curblockload);

     if(PHYSFS_exists(tex_path.c_str()) != 0) return;

     this->block_tex_id[curblockid] = load_texture(tex_path);
     
     this->progress += 1;
     if(this->progress >= this->total) return;
     double progress_percent = (this->progress / this->total);
     this->progress_w        = progress_percent * (this->loading_w);
}

void opencraft_appstate_ingame::update_connecting(SDL_Event *ev) {
     // TODO - pump appropriate network events here
}

void opencraft_appstate_ingame::update_playing(SDL_Event *ev) {
}

void opencraft_appstate_ingame::update_dead(SDL_Event *ev) {
}

void opencraft_appstate_ingame::render_loading() {
     draw_tiled_quad(this->bg_dirt_x,  this->bg_dirt_y,  this->bg_dirt_w,  this->bg_dirt_h,  BLOCK_TEXTURE_SIZE, BLOCK_TEXTURE_SIZE, this->dirtblock_gl_tex_id);
     draw_tiled_quad(this->bg_grass_x, this->bg_grass_y, this->bg_grass_w, this->bg_grass_h, BLOCK_TEXTURE_SIZE, BLOCK_TEXTURE_SIZE, this->grassblock_gl_tex_id);
     glEnable(GL_BLEND);
      draw_textured_quad(this->loading_x,    this->loading_y,    this->loading_w,    this->loading_h,    this->loading_tex_id);
      draw_textured_quad(this->loading_x,    this->loading_y+(this->loading_h), this->progress_w, this->loading_h, this->progress_gl_tex_id);
     glDisable(GL_BLEND);
}

void opencraft_appstate_ingame::render_connecting() {
}

void opencraft_appstate_ingame::render_playing() {
}

void opencraft_appstate_ingame::render_dead() {
}

void opencraft_appstate_ingame::render() {
     switch(this->cur_state) {
        case INGAME_LOADING: {
             this->render_loading();
        break;}
        case INGAME_CONNECTING: {
             this->render_connecting();
        break;}
        case INGAME_PLAYING: {
             this->render_playing();
        break;}
        case INGAME_DEAD: {
             this->render_dead();
        break;}
     }
}

opencraft_appstate_ingame::~opencraft_appstate_ingame() {
    // TODO - unload textures and crap here
    // TODO - return back to main menu
}
