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
//     BaseCLI class
//
//-----------------------------------------------------------------------------


#include <opencraft/appfw/interfaces/cli/base_cli.h>

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

namespace opencraft { namespace appfw { namespace interfaces { namespace cli {

void BaseCLI::Update() {
     if(first_update) {
        std::cout << this->app->Console->get_output();
        this->app->Console->add_listener(this);
        first_update = false;
     }
}

void BaseCLI::GrabEvents() {
     // this is a bit of a hack - basically we check using select() if stdin is open for reading and if so call readline, readline then handles passing full lines to the console
     fd_set rfds;
     struct timeval tv;
     tv.tv_sec  = 0;
     tv.tv_usec = 0;

     FD_ZERO(&rfds);
     FD_SET(0, &rfds);
     int retval = select(1, &rfds,NULL,NULL,&tv);
     if(FD_ISSET(0,&rfds)) {
        std::string s;
        std::getline(std::cin, s);
        this->app->Console->add_input(s);

     }
     std::cout << std::flush;
}

void BaseCLI::on_output(std::string s) {
     std::cout << s;
}

}}}};
