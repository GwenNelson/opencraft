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
//     Grabs JSON data from a server and spits it to stdout
//     In keeping with unix tradition, prints out nothing else
//     This could be integrated fairly trivially with a webservice
//
//-----------------------------------------------------------------------------


#include <libopencraft/packets.autogen.h>
#include <libopencraft/version.h>
#include <libopencraft/packet_reader.h>
#include <libopencraft/packet_writer.h>
#include <libopencraft/proto_constants.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <exception>
#include <vector>

using namespace std;
using namespace opencraft::packets;

int main(int argc, char** argv) {
    // setup socket
    int sockfd = socket(AF_INET, SOCK_STREAM,0);
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(25565);

    // connect to localhost:25565 and then setup a packet reader and writer
    connect(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in));
    opencraft::packets::packet_reader client_reader(sockfd,OPENCRAFT_STATE_STATUS,true);
    opencraft::packets::packet_writer client_writer(sockfd);

    // create a handshake packet and write it
    handshake_handshaking_upstream hspack(OPENCRAFT_PROTOCOL_VERSION,std::string(OPENCRAFT_DEFAULT_SERVER),OPENCRAFT_DEFAULT_TCP_PORT,OPENCRAFT_STATE_STATUS);
    client_writer.write_pack(&hspack);

    // create a status request packet and write it
    status_request_status_upstream status_req;
    client_writer.write_pack(&status_req);

    // read the next packet and dump it
    opencraft_packet* inpack = client_reader.read_pack();
    cout << inpack->name() << endl;
    cout << ((status_response_status_downstream*)inpack)->a;
    delete inpack;

    close(sockfd);
}




