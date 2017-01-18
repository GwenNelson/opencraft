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
}

void opencraft_appstate_menu::update_state(SDL_Event *ev) {
}

void opencraft_appstate_menu::render() {
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glEnable( GL_BLEND ); glClearColor(0.0,0.0,0.0,0.0);
     draw_tiled_quad(this->bg_dirt_x,  this->bg_dirt_y,  this->bg_dirt_w,  this->bg_dirt_h,  BLOCK_TEXTURE_SIZE, BLOCK_TEXTURE_SIZE, this->dirtblock_gl_tex_id);
     draw_tiled_quad(this->bg_grass_x, this->bg_grass_y, this->bg_grass_w, this->bg_grass_h, BLOCK_TEXTURE_SIZE, BLOCK_TEXTURE_SIZE, this->grassblock_gl_tex_id);

     glEnable(GL_BLEND);
     draw_textured_quad(this->logo_x, this->logo_y, this->logo_w, this->logo_h, this->title_gl_tex_id);
     glDisable(GL_BLEND);
}

