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
//     BaseConsole class
//
//-----------------------------------------------------------------------------

#include <opencraft/common.h>
#include <vector>
#include <string>
#include <iostream>
#include <opencraft/appfw/console/base_console.h>

namespace opencraft { namespace appfw { namespace console {

BaseConsole::BaseConsole() {
}

void BaseConsole::add_listener(ConsoleEventListener *l) {
     this->listeners.push_back(l);
}

void BaseConsole::clear_output() {
     this->outputbuf = "";
     for(auto const& l: this->listeners) {
            l->on_output_clear();
     }
}

void BaseConsole::add_output(std::string s) {
     this->outputbuf.append(s);
     for(auto const& l: this->listeners) {
            l->on_output(s);
     }
}

std::string BaseConsole::get_output() {
     return std::string(this->outputbuf);
}

void BaseConsole::clear_input() {
     this->inputbuf.clear();
     for(auto const& l: this->listeners) {
            l->on_input_clear();
     }
}

void BaseConsole::add_input(std::string s) {
     this->inputbuf.push_back(s);
     for(auto const& l: this->listeners) {
            l->on_input(s);
     }
}

std::vector<std::string> BaseConsole::get_input() {
     std::vector<std::string> retval;
     return retval;
}


}}};
