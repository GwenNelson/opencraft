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
//     BaseLogger class
//
//-----------------------------------------------------------------------------

#include <opencraft/common.h>

#include <opencraft/appfw/console/base_console.h>
#include <opencraft/appfw/console/logging/base_logger.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>
#include <string>

namespace pt = boost::posix_time;

using namespace std;

namespace opencraft { namespace appfw { namespace console { namespace logging {

BaseLogger::BaseLogger(opencraft::appfw::console::BaseConsole *_Console) {
     this->Console = _Console;
}

void BaseLogger::info(std::string s) {
     this->Console->add_output(pt::to_iso_extended_string(pt::second_clock::local_time()));
     this->Console->add_output(" [INFO] ");
     this->Console->add_output(s);
     this->Console->add_output("\n");
}

}}}};

