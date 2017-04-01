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
//     A class for handling the connection to the server
//
//-----------------------------------------------------------------------------


#include <common.h>
#include <opencraft_connection.h>
#include <SDL.h>

using namespace opencraft::packets;

extern int pack_event;

opencraft_connection::opencraft_connection(std::string server_addr, int port_no) {
    this->server_hostname = server_addr;
    this->server_port     = port_no;
    this->base            = event_base_new();
    this->dns_base        = evdns_base_new(this->base, 1);
    this->proto_mode      = OPENCRAFT_STATE_HANDSHAKING;
}

void opencraft_connection::send_packet(opencraft_packet* pack) {
     raw_packet rawpack(pack->ident(),pack->pack());
     std::vector<unsigned char> packed = rawpack.pack();
     evbuffer_add(bufferevent_get_output(this->bev),packed.data(),packed.size());
     LOG(debug) << "Sent " << pack->name() << " to server";
}

void opencraft_connection::read_cb(struct bufferevent *bev) {
     struct evbuffer *input = bufferevent_get_input(bev);
     size_t buflen          = evbuffer_get_length(input);

     unsigned char *n;
     if(buflen >= 5) {
        n = evbuffer_pullup(input, 5); // peek at the first 5 bytes
     } else {
        return;
     }

     int pack_size=0;
     
     for(int i=0; i<5; ++i) {
           pack_size |= (n[i] & 0x7F) << (i*7);
           if(!(n[i] & 0x80)) {
               break;
           }
     }

     
     if(buflen < (pack_size)) return;

     // if we do it's time to build a packet
     unsigned char* packbuf = (unsigned char*)calloc(pack_size,1);

     evbuffer_drain(input,varint_size(pack_size));
     size_t bytes_recv = bufferevent_read(bev,(void*)packbuf,pack_size);
     

     std::vector<unsigned char> tmpbuf;
     tmpbuf.assign(packbuf, packbuf+pack_size);
     free(packbuf);

     opencraft_packet* inpack = opencraft_packet::unpack_packet(this->proto_mode,true,tmpbuf);

     if(inpack->name().compare("unknown")==0) return;

     LOG(debug) << "Received " << inpack->name() << " from server, hex: " << inpack->dump_hex();

     // deleting the packet object is up to the main appstate code

     SDL_Event inpack_event;
     SDL_memset(&inpack_event, 0, sizeof(inpack_event));
     inpack_event.type       = pack_event;
     inpack_event.user.code  = this->proto_mode;
     inpack_event.user.data1 = (void*)inpack; // ident and such can be extracted from here
     inpack_event.user.data2 = 0;
     SDL_PushEvent(&inpack_event);
}

void readcb(struct bufferevent *bev, void *ptr) {
     opencraft_connection* oc_conn = (opencraft_connection*)ptr;
     oc_conn->read_cb(bev);
}

void eventcb(struct bufferevent *bev, short events, void *ptr) {

    if (events & BEV_EVENT_CONNECTED) {
         LOG(info) << "Connected";
    } else if (events & (BEV_EVENT_ERROR|BEV_EVENT_EOF)) {
         LOG(error) << std::string(evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
         struct event_base *base = (struct event_base*)ptr;
         if (events & BEV_EVENT_ERROR) {
                 int err = bufferevent_socket_get_dns_error(bev);
                 if (err)
                         LOG(error) << "DNS error: " << std::string(evutil_gai_strerror(err));
         }
         bufferevent_free(bev);
         event_base_loopexit(base, NULL);
    }
}


void opencraft_connection::connect() {
     this->bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
     bufferevent_setcb(this->bev, readcb, NULL, eventcb, this);
     bufferevent_enable(this->bev, EV_READ|EV_WRITE);
     bufferevent_socket_connect_hostname(this->bev, this->dns_base, AF_UNSPEC, this->server_hostname.c_str(), this->server_port);
     LOG(info) << "Sent connection request to " << this->server_hostname << ":" << std::to_string(this->server_port);
}

void opencraft_connection::pump_net() {
     event_base_loop(this->base, EVLOOP_ONCE|EVLOOP_NONBLOCK);
}
