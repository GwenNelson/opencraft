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


#include <client_connection.h>
#include <stdlib.h>
#include <vector>
#include <libopencraft/base_packet.h>
#include <libopencraft/packets.autogen.h>
#include <libopencraft/proto_constants.h>
#include <libopencraft/raw_packet.h>

#include <opencraft_daemon.h>

using namespace opencraft::packets;

client_connection::client_connection(std::string client_addr,opencraft_daemon* daemon) {
     this->_client_addr = client_addr;
     this->_daemon      = daemon;
     
     this->_recvbuf     = evbuffer_new();
     evbuffer_enable_locking(this->_recvbuf,NULL); // we need this threadsafe
     
     this->_sendbuf     = evbuffer_new();
     evbuffer_enable_locking(this->_sendbuf,NULL);

     this->proto_mode   = OPENCRAFT_STATE_HANDSHAKING;
}

void client_connection::write_cb(evutil_socket_t fd) {
     if(evbuffer_get_length(this->_sendbuf)==0) return;
     int bytes_sent = evbuffer_write(this->_sendbuf,fd);
     if(bytes_sent==0) return;
     if(bytes_sent==-1) {
        this->close();
     }
}

void client_connection::close() {
     event_del(this->read_ev);
     event_del(this->write_ev);
     event_free(this->read_ev);
     event_free(this->write_ev);
     this->_daemon->_inactive_clients.push_back(this->_client_addr);
     LOG(info) << this->_client_addr << " closing";
}

void client_connection::send_packet(opencraft_packet* pack) {
     raw_packet rawpack(pack->ident(),pack->pack());
     std::vector<unsigned char> packed = rawpack.pack();
     evbuffer_add(this->_sendbuf,packed.data(),packed.size());
     LOG(debug) << this->_client_addr << " sent " << pack->name() << " to client";
}

void client_connection::read_cb(evutil_socket_t fd) {
     boost::mutex::scoped_lock l(this->_mtx);
     this->_read_cb(fd);
}

void client_connection::_read_cb(evutil_socket_t fd) {
     // read as much as we can from the socket into recvbuf
     int recv_bytes = evbuffer_read(this->_recvbuf, fd, -1);
     if(recv_bytes <= 0) return;
     int buflen = evbuffer_get_length(this->_recvbuf);
     
     // try to grab the packet size
     unsigned char *n;
     if(buflen > 5) {
        n = evbuffer_pullup(this->_recvbuf, 5); // peek at the first 5 bytes
     } else {
        n = evbuffer_pullup(this->_recvbuf, buflen);
     }
     
     int pack_size=0;
     
     if(n[0]==0) {
        pack_size=0;
     } else {
       for(int i=0; i<5; ++i) {
           pack_size |= (n[i] & 0x7F) << (i*7);
           if(!(n[i] & 0x80)) {
               break;
           }
        }
     }

     LOG(debug) << this->_client_addr << " pack size = " << pack_size;
     
     // check if we have at least pack_size bytes, if not return
     if(buflen < pack_size) return;

     pack_size += varint_size(pack_size);
     // if we do it's time to build a packet
     unsigned char* packbuf = (unsigned char*)calloc(pack_size,1);
     evbuffer_remove(this->_recvbuf,(void*)packbuf,pack_size);

     std::vector<unsigned char> tmpbuf;
     tmpbuf.assign(packbuf, packbuf+pack_size);
     free(packbuf);

     opencraft::packets::opencraft_packet* pack = opencraft::packets::opencraft_packet::unpack_packet(this->proto_mode,false,tmpbuf);
     if(pack != NULL) { // we handle some packets here because some packets must be sorted immediately and the thread pool would cause delays
        LOG(debug) << this->_client_addr << " received " << pack->name() << " from client";
        int32_t pack_ident = pack->ident();
        if(this->proto_mode == OPENCRAFT_STATE_HANDSHAKING) {
           if(pack_ident != OPENCRAFT_PACKIDENT_HANDSHAKE_HANDSHAKING_UPSTREAM) {
              LOG(error) << this->_client_addr << " Got invalid handshake packet";
           }
           this->handle_handshake((handshake_handshaking_upstream*)pack);
           free(pack);
           return;
        } 
        
        if(this->proto_mode == OPENCRAFT_STATE_STATUS) {
           switch(pack_ident) {
                    case OPENCRAFT_PACKIDENT_STATUS_REQUEST_STATUS_UPSTREAM: {
                         status_response_status_downstream status_resp(this->_daemon->get_status_json());
                         this->send_packet(&status_resp);
                    break;}
                    case OPENCRAFT_PACKIDENT_STATUS_PING_STATUS_UPSTREAM: {
                         int32_t ack = ((status_ping_status_upstream*)pack)->a;
                         status_pong_status_downstream ack_pack(ack);
                         this->send_packet(&ack_pack);
                         this->close();
                    break;}

           }
        } else if(this->proto_mode == OPENCRAFT_STATE_LOGIN){
          switch(pack_ident) {
                     case OPENCRAFT_PACKIDENT_LOGIN_START_LOGIN_UPSTREAM: {
                     this->handle_login((login_start_login_upstream*)pack);
                     break;}

          }
        }

        if(this->proto_mode == OPENCRAFT_STATE_PLAY) {
           this->handle_packet(pack);
        }

        free(pack);
     }
}

void client_connection::handle_login(login_start_login_upstream* login_pack) {
     LOG(info) << this->_client_addr << " Attempting to login with username " << login_pack->a;

     // ask the daemon for a user profile for the specified username
     user_profile* profile = this->_daemon->profile_from_username(login_pack->a);
     if(profile==NULL) {
        // TODO - return an error here and disconnect the client for invalid/unknown users
        // TODO - also check if user is already connected with that name and disconnect with appropriate error
     }
     
     this->_profile = profile;

     login_success_login_downstream succ_pack(boost::uuids::to_string(profile->uuid),profile->username);
     this->send_packet(&succ_pack);
     this->proto_mode = OPENCRAFT_STATE_PLAY;

     join_game_play_downstream join_pack(profile->entity_id,profile->game_mode,profile->dimension,profile->difficulty,100,"default",false);
     this->send_packet(&join_pack);

     spawn_position_play_downstream spawn_pos(this->_daemon->get_spawn_pos());
     this->send_packet(&spawn_pos);

     player_position_and_look_play_downstream initial_pos(profile->pos_x, profile->pos_y, profile->pos_z,profile->yaw,profile->pitch,profile->on_ground,666);
     this->send_packet(&initial_pos);

}

void client_connection::handle_handshake(handshake_handshaking_upstream* hspack) {
     int32_t client_proto_version           = hspack->a;
     std::string server_addr                = hspack->b;
     uint16_t server_port                   = hspack->c;
     int32_t next_state                     = hspack->d;
     LOG(info) << this->_client_addr << " Received handshake";

     this->proto_mode = next_state;
}

void client_connection::handle_packet(opencraft_packet* pack) {

}
