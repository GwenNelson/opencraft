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
//     BaseShell class
//
//-----------------------------------------------------------------------------


#include <opencraft/common.h>
#include <vector>
#include <string>

#include <opencraft/appfw/console/console_event_listener.h>
#include <opencraft/appfw/appfw.h>

#include <opencraft/appfw/console/cmdshell/base_shell.h>

namespace opencraft { namespace appfw { namespace console { namespace cmdshell {

BaseShell::BaseShell(opencraft::appfw::App* _app) {
    this->app = _app;
    this->app->Console->add_listener(this);
}

void BaseShell::on_output(std::string s) {
}

void BaseShell::on_output_clear() {
}

void BaseShell::on_input(std::string s) {
     if(s.size()==0) return; // ignore empty lines
     OC_LOG_DEBUG(this->app, std::string("Got command: ") + s);
     this->app->Console->clear_input();
}

void BaseShell::on_input_clear() {
}

}}}};
