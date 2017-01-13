#include <iostream>
#include <string>

#include <wordexp.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

#include <boost/asio/io_service.hpp>
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

bool debug_mode  = false;
bool daemon_mode = true;

namespace po = boost::program_options;

void fork_me() {
     if(fork() > 0) exit(0);
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

void configure_daemon_signals() {
     sigset_t signal_set;
     sigemptyset(&signal_set);
     sigaddset(&signal_set, SIGCHLD);
     sigaddset(&signal_set, SIGTSTP);
     sigaddset(&signal_set, SIGTTOU);
     sigaddset(&signal_set, SIGTTIN);
     sigprocmask(SIG_BLOCK, &signal_set, NULL);
}

int main(int argc, char **argv) {
    int i;
    po::options_description desc("Options");

    desc.add_options() ("help,h",       "display this help")
                       ("foreground,f", "don't daemonize")
                       ("debug,d",      "run in debug mode")
                       ("pidfile,p",     po::value<string>()->default_value("~/.opencraftd.pid"),
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

    if(daemon_mode) {
       BOOST_LOG_TRIVIAL(info) << "Daemonizing...";
       write_pidfile(vm["pidfile"].as<string>());
       setup_daemon();
       configure_daemon_stdio();
       configure_daemon_signals();
    }

    BOOST_LOG_TRIVIAL(info) << "Starting server...";
    for(;;);
    return 0;
}
