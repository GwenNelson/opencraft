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

opencraft_connection::opencraft_connection(std::string server_addr, int port_no) {
    this->server_hostname = server_addr;
    this->server_port     = port_no;
    this->base            = event_base_new();
    this->dns_base        = evdns_base_new(this->base, 1);
}


void readcb(struct bufferevent *bev, void *ptr) {
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
     bufferevent_setcb(this->bev, readcb, NULL, eventcb, base);
     bufferevent_enable(this->bev, EV_READ|EV_WRITE);
     bufferevent_socket_connect_hostname(this->bev, this->dns_base, AF_UNSPEC, this->server_hostname.c_str(), this->server_port);
     LOG(info) << "Sent connection request to " << this->server_hostname << ":" << std::to_string(this->server_port);
     this->pump_net();
}

void opencraft_connection::pump_net() {
     event_base_loop(this->base, EVLOOP_NONBLOCK|EVLOOP_ONCE);
}
