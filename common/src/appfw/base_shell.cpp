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

// TODO: implement dynamic command magic
// TODO: basic commands: set,help,quit


#include <opencraft/common.h>
#include <vector>
#include <string>
#include <iostream>
#include <opencraft/appfw/console/console_event_listener.h>
#include <opencraft/appfw/appfw.h>

#include <opencraft/appfw/console/cmdshell/base_shell.h>

#include <boost/bind.hpp>
#include <boost/tokenizer.hpp>

// shamelessly ripped from a stackoverflow post
// attribution in accordance with the CC-By-SA license: http://stackoverflow.com/questions/18378798/use-boost-program-options-to-parse-an-arbitrary-string
std::vector<std::string> tokenize(const std::string& input)
{
  typedef boost::escaped_list_separator<char> separator_type;
  separator_type separator("\\",    // The escape characters.
                           "= ",    // The separator characters.
                           "\"\'"); // The quote characters.

  // Tokenize the intput.
  boost::tokenizer<separator_type> tokens(input, separator);

  // Copy non-empty tokens from the tokenizer into the result.
  std::vector<std::string> result;
  copy_if(tokens.begin(), tokens.end(), std::back_inserter(result), 
          !boost::bind(&std::string::empty, _1));
  return result;
}

namespace opencraft { namespace appfw { namespace console { namespace cmdshell {

void help_func(po::variables_map vm, opencraft::appfw::App *app, BaseShell* shell) {
     std::string cmdname;
     if(vm.count("cmd")==0) {
         for (auto const& it : shell->cmd_basic_usage) {
              cmdname  = it.first;
              std::string cmdusage = it.second;
              std::cout << cmdname << " \t\t" << cmdusage << std::endl;
         }
     } else {
         cmdname = vm["cmd"].as<std::string>();
         if(shell->cmd_opts.find(cmdname) != shell->cmd_opts.end()) {
            std::cout << shell->cmd_basic_usage[cmdname] << std::endl << std::endl;
            std::cout << shell->cmd_full_usage[cmdname] << std::endl;
         } else {
            std::cout << "Unknown command!" << std::endl;
         }
     }
}

void quit_func(po::variables_map vm, opencraft::appfw::App *app, BaseShell* shell) {
     app->FSM->Switch("EndProgramState");
}

BaseShell::BaseShell(opencraft::appfw::App* _app) {
    this->app = _app;
    this->app->Console->add_listener(this);

    // add builtin help command
    po::options_description *help_opts = new po::options_description("");
    help_opts->add_options()
       ("cmd", po::value< std::string>(), "command to get help for");
    po::positional_options_description *help_pos_opts = new po::positional_options_description();
    help_pos_opts->add("cmd", 1);
    this->add_cmd("help","Displays help for commands",
                  "help [cmdname]\n"
                  "     [cmdname] command to get help for, optional",
                   help_func,help_opts,help_pos_opts);
    
    // add builtin quit command
    po::options_description             *quit_opts      = new po::options_description("");
    po::positional_options_description  *quit_pos_opts  = new po::positional_options_description();
    this->add_cmd("quit","Closes the program","quit",quit_func,quit_opts,quit_pos_opts);

}

void BaseShell::on_output(std::string s) {
}

void BaseShell::on_output_clear() {
}

void BaseShell::add_cmd(std::string cmdname, std::string basic_usage, std::string full_usage, cmdshell_cmd_t cmdfunc, po::options_description *desc, po::positional_options_description* pos_opts_desc) {
     this->cmd_opts[cmdname]        = desc;
     this->cmd_pos_opts[cmdname]    = pos_opts_desc;
     this->cmd_funcs[cmdname]       = cmdfunc;
     this->cmd_basic_usage[cmdname] = basic_usage;
     this->cmd_full_usage[cmdname]  = full_usage;
}

void BaseShell::on_input(std::string s) {
     if(s.size()==0) return; // ignore empty lines

     std::vector<std::string> tokens = tokenize(s);

     std::string cmdname = std::string(tokens[0]);
     tokens.erase(tokens.begin());

     OC_LOG_DEBUG(this->app,std::string("Got command ") + cmdname);

     if(this->cmd_funcs.find(cmdname) != this->cmd_funcs.end()) {
        po::variables_map vm;
        po::store(po::command_line_parser(tokens)
                .options(*(this->cmd_opts[cmdname])).positional(*(this->cmd_pos_opts[cmdname])).run(), vm);
        po::notify(vm);
        this->cmd_funcs[cmdname](vm,this->app,this);
     }

     this->app->Console->clear_input();
}

void BaseShell::on_input_clear() {
}

}}}};
