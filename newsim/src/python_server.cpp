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
//     newsim python entry point
//
//-----------------------------------------------------------------------------

#include <boost/python.hpp>
#include <iostream>
#include <frameobject.h>

#include <libgen.h>

using namespace boost::python;

int init_python(char* mainfile) {
    try {
      Py_Initialize();
      PyObject *sys = PyImport_ImportModule("sys");
      PyObject *path = PyObject_GetAttrString(sys, "path");
      PyList_Append(path, PyString_FromString("."));
      PyList_Append(path, PyString_FromString(dirname(strdup(mainfile))));
      object main_module = import("__main__");
      dict main_namespace = extract<dict>(main_module.attr("__dict__"));
      exec_file(mainfile, main_namespace, main_namespace);
    } catch(error_already_set& e) {
      PyErr_PrintEx(0);
      return 1;
    }
    return 0;
}
