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
//     Console
//
//-----------------------------------------------------------------------------

#pragma once

void cmd_help(int argc, char** argv);
void cmd_set(int argc, char** argv);
void cmd_mount(int argc, char** argv);
void cmd_ls(int argc, char** argv);
void cmd_pwd(int argc, char** argv);
void cmd_cd(int argc, char** argv);
void cmd_cat(int argc, char** argv);
void cmd_debug(int argc, char** argv);

struct cons_cmd {
    char*   cmd_str;         // the actual command typed in the console
    char*   help_str;        // description of what the command does
    char*   param_list[3];   // list of parameters
    char*   param_desc[3];   // description of parameters
    char*   extra_help[3];   // extra help lines
    void    (*cmd_func)(int argc, char** argvv); // the function to invoke
};


unsigned int get_cmd_count();

void add_command(struct cons_cmd cmd);
void init_cmd_table();

