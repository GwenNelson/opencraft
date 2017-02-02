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
//     Class representing a client connection
//
//-----------------------------------------------------------------------------

#include <libopencraft/base_packet.h>
#include <libopencraft/packet_reader.h>
#include <libopencraft/packet_writer.h>
#include <libopencraft/proto_constants.h>
#include <libopencraft/packets.autogen.h>

#include <boost/random.hpp>
#include <iostream>
#include <ctime>
#include <cstdint>

#include <jsoncpp/json/value.h>
#include <jsoncpp/json/writer.h>

#include <client_connection.h>

using namespace opencraft::packets;

// TODO - move this elsewhere
std::string get_status_json() {
     Json::Value Version;
     Version["name"]     = "OpenCraft newsim";
     Version["protocol"] = OPENCRAFT_PROTOCOL_VERSION;
     
     Json::Value Players;
     Players["online"]   = 0;   // TODO: fix this to actually count clients connected
     Players["max"]      = 100; // TODO: make this a configurable variable

     Json::Value Description;
     Description["text"] = "OpenCraft server";

     Json::Value resp_val;
     resp_val["version"]     = Version;
     resp_val["players"]     = Players;
     resp_val["description"] = Description;
     
     Json::FastWriter Writer;
     std::string resp_str = Writer.write(resp_val);
     return resp_str;
}



client_connection::client_connection(int sock_fd, std::string client_addr) {
    this->proto_mode    = OPENCRAFT_STATE_HANDSHAKING;
    this->_sock_fd      = sock_fd;
    this->_client_addr  = client_addr;
    this->client_reader = new packet_reader(sock_fd,this->proto_mode,false);
    this->client_writer = new packet_writer(sock_fd);
    this->active        = true;
}

void client_connection::send_packet(opencraft::packets::opencraft_packet* pack) {
    packet_writer tmp_writer(this->_sock_fd);
    tmp_writer.write_pack(pack);
    LOG(debug) << this->_client_addr << " Sent " << pack->name();
}

void client_connection::handle_handshaking() {
     opencraft_packet *inpack = NULL;
     inpack = this->client_reader->read_pack();

     handshake_handshaking_upstream* hspack = inpack;
     int32_t client_proto_version           = hspack->a;
     std::string server_addr                = hspack->b;
     uint16_t server_port                   = hspack->c;
     int32_t next_state                     = hspack->d;
     
     LOG(info) << this->_client_addr << " Got handshake from client";
     this->proto_mode                = next_state;
     this->client_reader->proto_mode = next_state;
     delete inpack;
}

void client_connection::handle_status() {
     opencraft_packet *inpack = NULL;
     inpack = this->client_reader->read_pack();
        if(inpack != NULL) {
           if(inpack->name().compare("unknown")!=0) {
                LOG(debug) << this->_client_addr << " Received " << inpack->name();
                int32_t pack_ident = inpack->ident();
                switch(pack_ident) {
                    case OPENCRAFT_PACKIDENT_STATUS_REQUEST_STATUS_UPSTREAM: {
                         status_response_status_downstream status_resp(get_status_json());
                         this->send_packet(&status_resp);
                    break;}
                    case OPENCRAFT_PACKIDENT_STATUS_PING_STATUS_UPSTREAM: {
                         int32_t ack = ((status_ping_status_upstream*)inpack)->a;
                         status_pong_status_downstream ack_pack(ack);
                         this->send_packet(&ack_pack);
                         this->active=false; // close the connection
                    break;}
                 }
           }
        delete inpack;  
        }
}

void client_connection::handle_login() {
    opencraft_packet *inpack = NULL;
     inpack = this->client_reader->read_pack();
        if(inpack != NULL) {
           if(inpack->name().compare("unknown")!=0) {
                LOG(debug) << this->_client_addr << " Received " << inpack->name();
                int32_t pack_ident = inpack->ident();
                switch(pack_ident) {
                    case OPENCRAFT_PACKIDENT_LOGIN_START_LOGIN_UPSTREAM: {
                         login_start_login_upstream* login_pack = (login_start_login_upstream*)inpack;
                         LOG(info) << this->_client_addr << " Login from " << login_pack->a;
                         this->uuid     = boost::uuids::random_generator()();
                         this->username = std::string(login_pack->a);
                         LOG(info) << this->_client_addr << " User allocated UUID " << this->uuid;
                         login_success_login_downstream succ_pack(boost::uuids::to_string(this->uuid),this->username);
                         this->send_packet(&succ_pack);
                         this->client_reader->proto_mode = OPENCRAFT_STATE_PLAY;
                         this->proto_mode = OPENCRAFT_STATE_PLAY;

                         std::time_t now = std::time(0);
                         boost::random::mt19937 gen{static_cast<std::uint32_t>(now)};

                         this->entity_id  = gen();
                         this->game_mode  = OPENCRAFT_GAMEMODE_CREATIVE;
                         this->dimension  = OPENCRAFT_DIMENSION_OVERWORLD;
                         this->difficulty = OPENCRAFT_DIFFICULTY_PEACEFUL;
                         join_game_play_downstream join_pack(this->entity_id,this->game_mode,this->dimension,this->difficulty,100,"default",false);
                         this->send_packet(&join_pack);

                         spawn_position_play_downstream spawn_pos(std::tuple<int,int,int>(100,63,100));
                         this->send_packet(&spawn_pos);

                         player_position_and_look_play_downstream initial_pos(100.0, 63.0, 100.0,0.0f,0.0f,0,666);
                         this->send_packet(&initial_pos);
                    break;}
                 }
           }
        delete inpack;
        }

}

void client_connection::handle_play() {
    opencraft_packet *inpack = NULL;
     inpack = this->client_reader->read_pack();
        if(inpack != NULL) {
           if(inpack->name().compare("unknown")!=0) {
             LOG(debug) << this->_client_addr << " Received " << inpack->name();
             int32_t pack_ident = inpack->ident();
                switch(pack_ident) {
                 }
           }
        delete inpack;
        }
}

void client_connection::handle_client() {
     switch(this->proto_mode) {
        case OPENCRAFT_STATE_HANDSHAKING:
          this->handle_handshaking();
        break;
        case OPENCRAFT_STATE_STATUS:
          this->handle_status();
        break;
        case OPENCRAFT_STATE_LOGIN:
          this->handle_login();
        break;
        case OPENCRAFT_STATE_PLAY:
          this->handle_play();
        break;
     }
}

