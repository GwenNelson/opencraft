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

#include <unistd.h>
#include <sstream>
#include <vector>
#include <iostream>

#include <libopencraft/packets.autogen.h>
#include <libopencraft/version.h>
#include <libopencraft/packet_reader.h>
#include <libopencraft/packet_writer.h>
#include <libopencraft/proto_constants.h>

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

    this->conn_state = -1;
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

     predraw_text(default_font,255,255,255,(char*)"Connecting to server",&w,&h,&text_w,&text_h,&tex_id);
     this->connecting_w      = w;
     this->connecting_h      = h;
     this->connecting_text_w = text_w;
     this->connecting_text_h = text_h;
     this->connecting_tex_id = tex_id;
     this->connecting_x = (oc_video->res_w/2) - (this->connecting_text_w/2);
     this->connecting_y = (oc_video->res_h/2) - (this->connecting_text_h/2);




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
     this->total = (this->pending_loads.size()-1);
}

void opencraft_appstate_ingame::update_loading(SDL_Event *ev) {

     if(this->progress >= (this->total-1)) {
        this->total    = 4.0;
        this->progress = 0.0;
        LOG(info) << "Starting connection to server";
        this->cur_state = INGAME_CONNECTING;
        return;
     }


     pending_blockload_t curblockload = this->pending_loads.back();
     this->pending_loads.pop_back();
     
     std::string tex_path     = std::get<0>(curblockload);
     unsigned int curblockid  = std::get<1>(curblockload);

     if(PHYSFS_exists(tex_path.c_str()) != 0) return;

     this->block_tex_id[curblockid] = load_texture(tex_path);
     
     this->progress += 1;
     if(this->progress >= this->total) return;
     double progress_percent = (this->progress / this->total);
     this->progress_w        = progress_percent * (this->loading_w*2);
   }

void opencraft_appstate_ingame::update_connecting(SDL_Event *ev) {
     if(this->conn_state==-1) {  // is this the first time we're updating in the connecting state?
        this->total      = 7.0; // there's 4 basic stages when connecting
        this->progress   = 0.0;
        this->conn_state = INGAME_CONNECTING_SOCK_CONN;
     }
     
     if(this->conn_state > INGAME_CONNECTING_SOCK_CONN) {
        this->client_conn->pump_net();
     }
    
     switch(this->conn_state) {
        case INGAME_CONNECTING_SOCK_CONN:{
             std::string hostname;
             std::string port_no;
             std::istringstream sa_parsed(this->_server_addr);
             getline(sa_parsed,hostname,':');
             getline(sa_parsed,port_no,':');

             LOG(info) << "Connecting to host " << hostname << " on port " << port_no;

             this->client_conn   = new opencraft_connection(hostname,atoi(port_no.c_str()));
             this->progress     += 1.0;
             this->client_conn->connect();
             this->client_conn->pump_net();
             this->conn_state = INGAME_CONNECTING_HS;
        break;}

        case INGAME_CONNECTING_HS:{
             LOG(info) << "Sending handshake";
             handshake_handshaking_upstream hspack(OPENCRAFT_PROTOCOL_VERSION,this->client_conn->server_hostname,this->client_conn->server_port,OPENCRAFT_STATE_LOGIN);
             this->client_conn->send_packet(&hspack);
             this->progress += 1.0;
             this->conn_state = INGAME_CONNECTING_SENT_HS;
        break;}

        case INGAME_CONNECTING_SENT_HS:{
             LOG(info) << "Sending login";
             login_start_login_upstream login_req("OpenCraft user"); // TODO - add username option
             this->client_conn->send_packet(&login_req);
             this->conn_state = INGAME_CONNECTING_SENT_LOGIN;
        break;}

        case INGAME_CONNECTING_LOGIN_SUCC:{
             this->progress      += 1.0;
             this->conn_state = INGAME_CONNECTING_DOWNLOAD_TERRAIN;
             sleep(1);
        break;}

        case INGAME_CONNECTING_LOGIN_FAIL:{
             this->progress += 1.0;
             sleep(1);
        break;}

        case INGAME_CONNECTING_DOWNLOAD_TERRAIN:{
             this->progress      += 1.0;
        break;}
     }


     double progress_percent  = (this->progress / this->total);
     this->progress_w         = progress_percent * (this->loading_w*2);
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
      draw_textured_quad( (oc_video->res_w/2) - (this->progress_w/2),    this->loading_y+(this->loading_h), this->progress_w, this->loading_h/2, this->progress_gl_tex_id);
     glDisable(GL_BLEND);
}

void opencraft_appstate_ingame::render_connecting() {
     draw_tiled_quad(this->bg_dirt_x,  this->bg_dirt_y,  this->bg_dirt_w,  this->bg_dirt_h,  BLOCK_TEXTURE_SIZE, BLOCK_TEXTURE_SIZE, this->dirtblock_gl_tex_id);
     draw_tiled_quad(this->bg_grass_x, this->bg_grass_y, this->bg_grass_w, this->bg_grass_h, BLOCK_TEXTURE_SIZE, BLOCK_TEXTURE_SIZE, this->grassblock_gl_tex_id);
     glEnable(GL_BLEND);
      draw_textured_quad(this->connecting_x,    this->connecting_y,    this->connecting_w,    this->connecting_h,    this->connecting_tex_id);
      draw_textured_quad( (oc_video->res_w/2) - (this->progress_w/2),    this->connecting_y+(this->connecting_h), this->progress_w, this->connecting_h/2, this->progress_gl_tex_id);
     glDisable(GL_BLEND);

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
