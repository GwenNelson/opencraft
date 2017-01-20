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
//     Join game appstate - NOT the same as ingame
//
//-----------------------------------------------------------------------------

#include <common.h>
#include <opencraft_appstate_joingame.h>
#include <opencraft_video.h>
#include <r_2d.h>

#include <string>

extern opencraft_video *oc_video;
extern void* default_font;

opencraft_appstate_joingame::opencraft_appstate_joingame() {
    oc_video->enter_2d();
    // TODO: Write a base class for menu, options and join game
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

    this->add_entries();    
}

void opencraft_appstate_joingame::add_entry(std::string name, std::string address) {
   LOG(debug) << "Adding entry " << name << " at host address: " << address;
   unsigned int name_tex_id;
   unsigned int address_tex_id;
   int w;
   int h;
   int text_w;
   int text_h;
   predraw_text(default_font,255,255,255,(char*)address.c_str(),&w,&h,&text_w,&text_h,&address_tex_id);
   predraw_text(default_font,255,255,255,(char*)name.c_str(),&w,&h,&text_w,&text_h,&name_tex_id);
   joingame_entry_t new_entry(name,address,name_tex_id,address_tex_id,text_w,text_h);
   this->entries.push_back(new_entry);
}

void opencraft_appstate_joingame::add_entries() {
   this->add_entry("Local minecraft server", "127.0.0.1:25565");
}

opencraft_appstate_joingame::~opencraft_appstate_joingame() {
    glDeleteTextures(1, &(this->title_gl_tex_id));
    glDeleteTextures(1, &(this->dirtblock_gl_tex_id));
    glDeleteTextures(1, &(this->grassblock_gl_tex_id));
}

void opencraft_appstate_joingame::update_state(SDL_Event *ev) {
     int mouse_x;
     int mouse_y;
     int entry_x;
     int entry_y;
     int entry_w;
     int entry_h;
     SDL_GetMouseState(&mouse_x,&mouse_y);
     float last_y=this->logo_y;
     float last_h=this->logo_h;
     this->connect_to = std::string("");
     for(int i=0; i < this->entries.size(); i++) {
              entry_x = (oc_video->res_w/2) - (std::get<4>(this->entries[i])/2);
              entry_y = last_y+last_h+1.0f;
              entry_w = std::get<4>(this->entries[i]);
              entry_h = std::get<5>(this->entries[i]);
              last_y=last_y+last_h+1.0f+std::get<4>(this->entries[i]);
              last_h=std::get<4>(this->entries[i]);
              if((mouse_x>=entry_x) && (mouse_x<=(entry_x+entry_w)) && (mouse_y>=entry_y) && (mouse_y<=(entry_y+entry_h))) {
                 this->connect_to = std::get<1>(this->entries[i]);
              }
     }
     switch(ev->type) {
        case SDL_MOUSEBUTTONDOWN:
          LOG(info) << "Trying to connect to " << this->connect_to;
        break;
     }
}

void opencraft_appstate_joingame::render() {
     draw_tiled_quad(this->bg_dirt_x,  this->bg_dirt_y,  this->bg_dirt_w,  this->bg_dirt_h,  BLOCK_TEXTURE_SIZE, BLOCK_TEXTURE_SIZE, this->dirtblock_gl_tex_id);
     draw_tiled_quad(this->bg_grass_x, this->bg_grass_y, this->bg_grass_w, this->bg_grass_h, BLOCK_TEXTURE_SIZE, BLOCK_TEXTURE_SIZE, this->grassblock_gl_tex_id);

     glEnable(GL_BLEND);
     draw_textured_quad(this->logo_x,    this->logo_y,    this->logo_w,    this->logo_h,    this->title_gl_tex_id);
     float last_y=this->logo_y;
     float last_h=this->logo_h;

     unsigned int tex_id;
     for(int i=0; i< this->entries.size(); i++) {
         if(this->connect_to == std::get<1>(this->entries[i])) {
            tex_id = std::get<3>(this->entries[i]);
         } else {
            tex_id = std::get<2>(this->entries[i]);
         }
         draw_textured_quad((oc_video->res_w/2) - (std::get<4>(this->entries[i])/2),
                            last_y+last_h+1.0f,
                            std::get<4>(this->entries[i]),
                            std::get<5>(this->entries[i]),
                            tex_id);
         last_y=last_y+last_h+1.0f+std::get<4>(this->entries[i]);
         last_h=std::get<4>(this->entries[i]);
     }
     glDisable(GL_BLEND);
}

