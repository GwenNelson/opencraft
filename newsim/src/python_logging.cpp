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
//     Provide a means for python code to log to boost log
//
//-----------------------------------------------------------------------------

#include <common.h>
#include <boost/python.hpp>
#include <iostream>
#include <frameobject.h>

using namespace boost::python;

struct boost_log {
   void debug(std::string msg) {
        LOG(debug) << "[PYTHON] " << msg;
   }
   void info(std::string msg) {
        LOG(info) << "[PYTHON] " << msg;
   }
};

BOOST_PYTHON_MODULE(boost_log)
{
    class_<boost_log>("logger")
        .def("debug", &boost_log::debug)
        .def("info",  &boost_log::info)
    ;
};

void add_python_logging() {
     initboost_log();
}
