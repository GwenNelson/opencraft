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

#include <common.h>
#include <opencraft_console_cmds.h>
#include <opencraft_console.h>
#include <opencraft_video.h>

static struct cons_cmd builtin_commands[] = {
  {"help", "Displays usage information",
    {"command",NULL,NULL},
    {"the command to get usage info for",NULL,NULL},
    {NULL,NULL,NULL},
    &cmd_help},
  {"debug", "Toggles debug overlay",
    {NULL,NULL,NULL},
    {NULL,NULL,NULL},
    {"The debug overlay displays information such as FPS etc onscreen after rendering",NULL,NULL},
  &cmd_debug},
  {"set", "Sets a CVar to a specific value or displays CVars", 
    {"type","varname","value"},
    {"one of s,i,f or b",
     "variable to set",
     "value to set"},
    {"Data types supported: string(s), integer(i), float(f), boolean(b)",
     "For boolean values, valid values are: true, false",
     "Type set without params to see current CVars"},
   &cmd_set},
  {"mount","Mounts an archive in the VFS",
    {"archive","mountpoint",NULL},
    {"The archive or directory to mount",
     "Where in the VFS to mount it",
     NULL},
    {"Type mount without params to see currently mounted archives",
     "VFS support is implemented by PhysFS, see documentation for details on file formats supported",
     "Please note that .pk3 files are plain zip files"},
   &cmd_mount},
  {"ls","Lists a directory in the VFS",
    {"dir",NULL,NULL},
    {"The directory path to list, leave blank for current directory",
     NULL,
     NULL},
    {NULL,NULL,NULL},
   &cmd_ls},
  {"pwd","Prints the current directory to the console",
   {NULL,NULL,NULL},
   {NULL,NULL,NULL},
   {"This command simply displays the contents of the cwd CVar",NULL,NULL},
  &cmd_pwd}, 
  {"cd","Changes current working directory",
   {"dir",NULL,NULL},
   {"The directory to change to, do not add a trailing slash (/), may be relative to current directory or absolute",
    NULL,
    NULL},
   {NULL,NULL,NULL},
  &cmd_cd},
  {"cat","Dumps a file from the VFS to the console",
   {"filename",NULL,NULL},
   {"The file to display - this should be an ASCII text file",
    NULL,
    NULL},
   {"Please note that very small files or binary files may corrupt the console display",
    NULL,
    NULL},
   &cmd_cat},
};

struct cons_cmd* cons_commands=NULL;
static unsigned int cmd_count=0;

unsigned int get_cmd_count() {
     return cmd_count;
}

void add_command(struct cons_cmd cmd) {
     cmd_count++;
     cons_commands = (cons_cmd*)realloc((void*)cons_commands,(get_cmd_count())*sizeof(struct cons_cmd));
     memcpy(&(cons_commands[cmd_count-1]), &cmd, sizeof(struct cons_cmd));
}

void init_cmd_table() {
     int i=0;
     for(i=0; i< (sizeof(builtin_commands)/sizeof(struct cons_cmd)); i++) {
         add_command(builtin_commands[i]);
     }
}

void cmd_debug(int argc, char** argv) {
}

void cmd_cat(int argc, char** argv) {
}

void cmd_cd(int argc, char** argv) {
}

void cmd_pwd(int argc, char** argv) {
}

void cmd_ls(int argc, char** argv) {
}

void cmd_mount(int argc, char** argv) {
}

void cmd_set(int argc, char** argv) {
}

void cmd_help(int argc, char** argv) {
     int i,p;
     char* help_args[2] = {"help","-h"};
     if(argc==1) {
        console_printf("Following commands are available, type help <cmd> for more info:\n");
        for(i=0; i< get_cmd_count(); i++) {
            console_printf(" * %s - %s\n",cons_commands[i].cmd_str,cons_commands[i].help_str);
        }
     } else if(argc==2) {
        for(i=0; i< get_cmd_count(); i++) {
            if(strcmp(cons_commands[i].cmd_str,argv[1])==0) {
               console_printf("\n %s - %s\n\n",cons_commands[i].cmd_str,cons_commands[i].help_str);
               console_printf(" Usage: %s ",cons_commands[i].cmd_str);
               for(p=0; p<3; p++) {
                  if(cons_commands[i].param_list[p] != NULL) {
                     console_printf("<%s> ",cons_commands[i].param_list[p]);
                  }
               }
               console_printf("\n");
               for(p=0; p<3; p++) {
                  if(cons_commands[i].param_list[p] != NULL) {
                     console_printf("        <%s> - %s\n",cons_commands[i].param_list[p],cons_commands[i].param_desc[p]);
                  }
               }
               console_printf("\n");
               for(p=0; p<3; p++) {
                  if(cons_commands[i].extra_help[p] != NULL) {
                     console_printf("        %s\n",cons_commands[i].extra_help[p]);
                  }
               }
               console_printf("\n");
            }
        }
     }
}
