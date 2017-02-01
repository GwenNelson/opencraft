//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of OpenCraft.
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
//     Entry point for the test client and CLI
//
//-----------------------------------------------------------------------------

#include <libopencraft/version.h>
#include <libopencraft/packets.autogen.h>
#include <libopencraft/packet_writer.h>

#include <boost/program_options.hpp>
#include <boost/bind.hpp>
#include <boost/tokenizer.hpp>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <exception>
#include <vector>

#include "bg_client.h"

using namespace std;

namespace po      = boost::program_options;

bg_client* client;

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

bool running=true;

void login_cmd(std::string cmdparams) {
     po::options_description desc("Options");
     std::string server_addr;
     int         port;
     std::string username;
     desc.add_options()
        ("help,h","Help")
        ("server_addr,s",po::value<std::string>(&server_addr)->default_value("127.0.0.1"),"IP address or hostname of the server")
        ("server_port,p",po::value<int>(&port)->default_value(25565),                     "TCP port of the server")
        ("username,u",   po::value<std::string>(&username)->default_value("TestUser"),    "Username to login with");

     bool parse_error = false;
     po::variables_map vm;
     try {
        po::store(po::command_line_parser(tokenize(cmdparams)).options(desc).run(),vm);
        po::notify(vm);
     } catch(po::error &error) {
        parse_error = true;
     }

     if(parse_error || vm.count("help")) {
        cout << desc;
        return;
     }
     
     client = new bg_client(server_addr,port,username);
}

void say_cmd(std::string cmdparams) {
     opencraft::packets::chat_message_play_upstream chat_msg(cmdparams);
     client->client_writer->write_pack(&chat_msg);
}

void show_help(std::string cmdparams) {
     po::options_description desc("");
     std::string cmd;
     desc.add_options()("cmd", po::value<std::string>(&cmd));
     po::positional_options_description pos_options;
     pos_options.add("cmd",1);
     po::variables_map vm;

     po::store(po::command_line_parser(tokenize(cmdparams)).options(desc).positional(pos_options).run(),vm);
     po::notify(vm);
     
     if(vm.count("cmd")) {
        if(cmd=="login") login_cmd("-h");
     } else {
        cout << "help       - display this information\n";
        cout << "help [cmd] - more help on specific command\n";
        cout << "say  [msg] - say something ingame\n";
        cout << "login      - login to a minecraft/opencraft server\n";
        cout << "quit       - quit the program\n";
     }
}

int main(int argc, char** argv) {
    cout << LIBOPENCRAFT_LONG_VER << endl << "Built on " << LIBOPENCRAFT_BUILDDATE << endl << endl;

    std::string cmdparams("");
    std::string cmd("");
    while(running) {
       cmdparams.erase();
       cmd.erase();
       cout << "> ";
       cin >> cmd;
       getline(cin,cmdparams);
       if(cmd=="help")  show_help(cmdparams);
       if(cmd=="login") login_cmd(cmdparams);
       if(cmd=="say")   say_cmd(cmdparams);
       if(cmd=="quit") running=false;
    }

}




