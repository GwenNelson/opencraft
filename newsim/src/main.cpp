//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of OpenCraft.
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
//     Entry point for the newsim server
//
//-----------------------------------------------------------------------------

#include <iostream>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <zmqpp/zmqpp.hpp>

#include <libopencraft/version.h>
#include <libopencraft/packet_reader.h>
#include <libopencraft/packet_writer.h>
#include <libopencraft/packets.autogen.h>
#include <libopencraft/proto_constants.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORTNUM 25565

using namespace std;
using namespace opencraft::packets;

int server_sock_fd;
bool running=true;

void client_handler(int client_fd,struct sockaddr_in client_addr) {
     cout << "Got new connection from " << inet_ntoa(client_addr.sin_addr) << endl;

     packet_reader client_reader(client_fd,OPENCRAFT_STATE_HANDSHAKING,false);
     packet_writer client_writer(client_fd);

     // first packet must ALWAYS be handshake
     opencraft_packet* inpack = NULL;
     inpack = client_reader.read_pack();
     if(inpack->ident() != OPENCRAFT_PACKIDENT_HANDSHAKE_HANDSHAKING_UPSTREAM) {
        cerr << "Bad packet from " << inet_ntoa(client_addr.sin_addr) << endl;
        close(client_fd);
        return;
     }

     handshake_handshaking_upstream* hspack = inpack;
     int32_t client_proto_version           = hspack->a;
     std::string server_addr                = hspack->b;
     uint16_t server_port                   = hspack->c;
     int32_t next_state                     = hspack->d;

     if(client_proto_version != OPENCRAFT_PROTOCOL_VERSION) {
        cerr << "Unsupported protocol version from " << inet_ntoa(client_addr.sin_addr) << endl;
        close(client_fd);
        return;
     }

     client_reader.proto_mode = next_state;
     while(running) {
        inpack = NULL;
        inpack = client_reader.read_pack();
        if(inpack != NULL) cout << inpack->name() << endl;
     }
     close(client_fd);
}

void accept_client_thread() {
     cout << "Awaiting clients\n";
     socklen_t socksize = sizeof(struct sockaddr_in);
     while(running) {
        struct sockaddr_in client_addr;
        int client_sock = accept(server_sock_fd, (struct sockaddr*)&client_addr,&socksize);
        boost::thread client_t{boost::bind(client_handler,client_sock,client_addr)};

     }
}

void init_server_sock() {
     struct sockaddr_in serv;
     socklen_t socksize = sizeof(struct sockaddr_in);
     memset(&serv,0,sizeof(serv));
     serv.sin_family      = AF_INET;
     serv.sin_addr.s_addr = htonl(INADDR_ANY);
     serv.sin_port        = htons(PORTNUM);

     server_sock_fd = socket(AF_INET,SOCK_STREAM,0);
     bind(server_sock_fd, (struct sockaddr*)&serv, sizeof(struct sockaddr));
     listen(server_sock_fd,1);
}

int main(int argc, char** argv) {
    cout << LIBOPENCRAFT_LONG_VER << endl << "Built on " << LIBOPENCRAFT_BUILDDATE << endl << endl;

    init_server_sock();
    
    boost::thread accept_thr{accept_client_thread};
    for(;;);
}
