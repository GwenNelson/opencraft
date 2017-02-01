//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of OpenCraft.
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
//     Background client
//
//-----------------------------------------------------------------------------


#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <libopencraft/base_packet.h>
#include <libopencraft/packet_reader.h>
#include <libopencraft/packet_writer.h>
#include <libopencraft/packets.autogen.h>
#include <libopencraft/proto_constants.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

#include "bg_client.h"

using namespace opencraft::packets;
using namespace std;

bg_client::bg_client(std::string _server_addr, int _server_port, std::string _username) {
     this->server_addr   = _server_addr;
     this->server_port   = _server_port;
     this->username      = _username;


     // TODO: actually use server_addr
     this->sockfd = socket(AF_INET, SOCK_STREAM,0);
     struct sockaddr_in server_addr;
     memset(&server_addr, 0, sizeof(server_addr));
     server_addr.sin_family = AF_INET;
     server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
     server_addr.sin_port = htons(this->server_port);
     connect(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in));


     this->client_reader = new opencraft::packets::packet_reader(this->sockfd,OPENCRAFT_STATE_LOGIN,true);
     this->client_writer = new opencraft::packets::packet_writer(this->sockfd);
     this->active        = true;
     boost::thread *t    = new boost::thread(boost::bind(&bg_client::reader_thread,this));
}

void bg_client::reader_thread() {
     handshake_handshaking_upstream hspack(OPENCRAFT_PROTOCOL_VERSION,std::string(this->server_addr),this->server_port,OPENCRAFT_STATE_LOGIN);
     this->client_writer->write_pack(&hspack);

     login_start_login_upstream login_req(this->username);
     this->client_writer->write_pack(&login_req);

     login_success_login_downstream *login_succ = this->client_reader->read_pack();
 
     cout << "Got UUID " << login_succ->a << endl;

     this->client_reader->proto_mode = OPENCRAFT_STATE_PLAY;
     this->proto_mode                = OPENCRAFT_STATE_PLAY;
     
     while(this->active) {
        opencraft_packet* inpack = NULL;
        inpack = this->client_reader->read_pack();
        if(inpack != NULL) {
             if(inpack->name().compare("unknown")!=0) {
                cout << inpack->name() << endl;
                if(inpack->ident()==OPENCRAFT_PACKIDENT_KEEP_ALIVE_PLAY_DOWNSTREAM) {
                   int32_t ackval = ((keep_alive_play_downstream*)inpack)->a;
                   keep_alive_play_upstream ack_pack(ackval);
                   this->client_writer->write_pack(&ack_pack);
                }
                if(inpack->ident()==OPENCRAFT_PACKIDENT_PLAYER_POSITION_AND_LOOK_PLAY_DOWNSTREAM) {
                     int32_t teleport_id = ((player_position_and_look_play_downstream*)inpack)->g;
                     teleport_confirm_play_upstream teleport_pack(teleport_id);
                     this->client_writer->write_pack(&teleport_pack);
                }
                if(inpack->ident()==OPENCRAFT_PACKIDENT_CHAT_MESSAGE_PLAY_DOWNSTREAM) {
                      std::string msg = ((chat_message_play_downstream*)inpack)->a;
                     cout << msg << endl;
                }
          }

          delete inpack;
        }

     }
}


