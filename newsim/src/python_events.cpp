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
//     Provide a means for python code to send events to C++
//
//-----------------------------------------------------------------------------

#include <common.h>
#include <boost/python.hpp>
#include <iostream>
#include <tuple>
#include <frameobject.h>

using namespace boost::python;

struct cpp_events {
   void on_packet(int32_t event_id, std::string packet) {
        LOG(debug) << "Got packet event ID " << event_id << " of size " << packet.size();
   }
};

BOOST_PYTHON_MODULE(cpp_events)
{
    class_<cpp_events>("cpp_events")
        .def("on_packet", &cpp_events::on_packet)
    ;
};

void add_python_cpp_events() {
     initcpp_events();
}
