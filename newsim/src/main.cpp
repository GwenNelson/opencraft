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
//          Main entry point and daemon stuff
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <string>

#include <wordexp.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

#include <boost/asio/io_service.hpp>
#include <boost/program_options.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/thread/thread.hpp>

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <common.h>
#include <python_server.h>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

bool        debug_mode   = false;
bool        daemon_mode  = true;
int         thread_count = 0;
std::string install_root = "";

namespace po      = boost::program_options;
namespace logging = boost::log;

void shutdown_server();
void startup_server();


void fork_me() {
     if(fork() > 0) exit(0);
}

void configure_logging(std::string logfile) {
     if(daemon_mode) {
        logging::add_file_log
        (
          logging::keywords::file_name = logfile,
          logging::keywords::rotation_size = 10 * 1024 * 1024,
          logging::keywords::time_based_rotation = logging::sinks::file::rotation_at_time_point(0, 0, 0),
          logging::keywords::format = "[%TimeStamp%]: %Message%" 
        );
     }

     if(debug_mode) {
        logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::debug);
     } else {
        logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
     }
     logging::add_common_attributes();
}

void write_pidfile(std::string filename) {
     wordexp_t exp_result;
     wordexp(filename.c_str(), &exp_result, 0);
     FILE* fd = fopen(exp_result.we_wordv[0],"w");
     wordfree(&exp_result);
     fprintf(fd,"%d\n",getpid());
     fclose(fd);
}

void setup_daemon() {
     fork_me();
     chdir("/");
     setsid();
     umask(0);
     fork_me();
}

void configure_daemon_stdio() {
     int i;
     for (i = getdtablesize(); i >= 0; --i) {
            close(i);
     }
     i = open("/dev/null", O_RDWR);
     dup(i);
     dup(i);
}

void signal_handler(int sig) {
     switch(sig) {
        case SIGHUP: // TODO - add config reload here
          break;
  
        case SIGINT:
          break;
  
        case SIGTERM:
          shutdown_server();
          exit(0);
          break;
        default:
          LOG(warning) << "Unhandled signal: " << sig;
        break;
     }
}

void configure_daemon_signals() {
     sigset_t         signal_set;
     struct sigaction sig_action;

     sigemptyset(&signal_set);
     sigaddset(&signal_set, SIGCHLD);
     sigaddset(&signal_set, SIGTSTP);
     sigaddset(&signal_set, SIGTTOU);
     sigaddset(&signal_set, SIGTTIN);
     sigprocmask(SIG_BLOCK, &signal_set, NULL);

     sig_action.sa_handler = signal_handler;
     sigemptyset(&sig_action.sa_mask);

     sig_action.sa_flags = 0;
     sigaction(SIGHUP,  &sig_action, NULL);
     sigaction(SIGTERM, &sig_action, NULL);
     sigaction(SIGINT,  &sig_action, NULL);
}

void startup_server() {
     LOG(info) << "Starting server...";
    
     boost::asio::io_service io_service;
     boost::asio::io_service::work work(io_service);

     LOG(info) << "Using " << thread_count << " worker threads";

     // start the pool worker threads
     boost::thread_group threads;
     for (std::size_t i = 0; i < thread_count; ++i)
         threads.create_thread(boost::bind(&boost::asio::io_service::run, &io_service));


     // start the python thread
     std::string mainpy_path = install_root + std::string("/python/python_server.py");
     char* mainpy_path_      = (char*)mainpy_path.c_str();
     LOG(info) << "Starting " << mainpy_path;
     threads.create_thread(boost::bind(&init_python,mainpy_path_));

     io_service.run(); // after this we never return
}

void shutdown_server() {
}


int main(int argc, char **argv) {
    int i;
    cout << OPENCRAFT_LONG_VER << endl << "Built on " << OPENCRAFT_BUILDDATE << endl;
    po::options_description desc("Options");

    desc.add_options() ("help,h",       "display this help")
                       ("foreground,f", "don't daemonize")
                       ("debug,d",      "run in debug mode")
                       ("workers,w",    po::value<int>()->default_value(4),
                                        "how many worker threads to use in the main threadpool")
                       ("logfile,l",    po::value<string>()->default_value("./opencraftd.log.%N"),
                                        "log to file")
                       ("root,r",       po::value<string>()->default_value("."),
                                        "path to install root")
                       ("pidfile,p",    po::value<string>()->default_value("~/.opencraftd.pid"),
                                        "path to the PID file for daemon");

    po::variables_map vm;
    
    bool parse_error = false;

    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (po::error &error) {
        parse_error = true;
    }

    if(parse_error || vm.count("help")==1) {
       cout << "Usage: opencraftd [options]\n" << desc << endl;
       return 0;
    }
       
    if(vm.count("debug")==1) {
       debug_mode = true;
    }
    if(vm.count("foreground")==1) {
       daemon_mode = false;
    }

    configure_logging(vm["logfile"].as<string>());

    thread_count = vm["workers"].as<int>();

    install_root = vm["root"].as<string>();

    if(daemon_mode) {
       LOG(info) << "Daemonizing...";
       write_pidfile(vm["pidfile"].as<string>());
       setup_daemon();
       configure_daemon_stdio();
       configure_daemon_signals();
    }

    startup_server();
    for(;;);
    return 0;
}
