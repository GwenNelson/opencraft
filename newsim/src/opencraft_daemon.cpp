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
//     The daemon class
//
//-----------------------------------------------------------------------------

#include <logging.h>
#include <opencraft_daemon.h>
#include <client_connection.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <wordexp.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <string>

using namespace std;

opencraft_daemon::opencraft_daemon(bool debug_mode, bool daemon_mode, std::string pidfile, std::string install_root, int listen_port) {
   this->_debug_mode      = debug_mode;
   this->_daemon_mode     = daemon_mode;
   this->_pidfile         = pidfile;
   this->_install_root    = install_root;
   this->_listen_port     = listen_port;
   this->_active          = false;
}

void opencraft_daemon::handle_client(int client_sock_fd, struct sockaddr_in client_addr) {
     LOG(info) << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port);
     std::string c_addr = std::string(inet_ntoa(client_addr.sin_addr)) + ":" + to_string(client_addr.sin_port);
     client_connection *client = new client_connection(client_sock_fd,c_addr);

     while(client->active) client->handle_client();
     LOG(info) << client->_client_addr << " Client disconnected";
     delete client;
     close(client_sock_fd);
}

void opencraft_daemon::accept_clients() {
     socklen_t socksize = sizeof(struct sockaddr_in);
     boost::thread_group client_threads;
     while(this->_active) {
        struct sockaddr_in client_addr;
        int client_sock = accept(this->_server_sock_fd, (struct sockaddr*)&client_addr,&socksize);

        client_threads.create_thread(boost::bind(&opencraft_daemon::handle_client,this,client_sock,client_addr));
     }
     LOG(debug) << "We should not be here, we are the thing that should not be, get in touch with lovecraft now";
}

void opencraft_daemon::run() {
     this->write_pidfile(this->_pidfile);
     if(this->_daemon_mode) {
        LOG(info) << "Daemonizing...";
        this->setup_daemon();
        this->configure_stdio();
        this->configure_signals();
     }
     
     LOG(info) << "Setting up listener port";

     struct sockaddr_in serv;
     socklen_t socksize = sizeof(struct sockaddr_in);
     memset(&serv,0,sizeof(serv));
     serv.sin_family      = AF_INET;
     serv.sin_addr.s_addr = htonl(INADDR_ANY);
     serv.sin_port        = htons(this->_listen_port);

     this -> _server_sock_fd = socket(AF_INET,SOCK_STREAM,0);
     setsockopt(this->_server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
     bind(this->_server_sock_fd, (struct sockaddr*)&serv, sizeof(struct sockaddr_in));
     listen(this->_server_sock_fd,1);
     this->_active = true;
     boost::thread accept_thr{boost::bind(&opencraft_daemon::accept_clients, this)};
}

void opencraft_daemon::fork_me_baby() { // harder, yeah
     if(fork() >0) exit(0);
}

void opencraft_daemon::write_pidfile(std::string filename) {
     wordexp_t exp_result;
     wordexp(filename.c_str(), &exp_result, 0);
     FILE* fd = fopen(exp_result.we_wordv[0],"w");
     wordfree(&exp_result);
     fprintf(fd,"%d\n",getpid());
     fclose(fd);
}

void opencraft_daemon::setup_daemon() {
     this->fork_me_baby();
     chdir("/");
     setsid();
     umask(0);
     this->fork_me_baby(); // fork me harder, oh yeah
}

void opencraft_daemon::configure_stdio() {
     int i;
     for (i = getdtablesize(); i >= 0; --i) {
            close(i);
     }
     i = open("/dev/null", O_RDWR);
     dup(i);
     dup(i);
}

void opencraft_daemon::configure_signals() {
     sigset_t         signal_set;
     struct sigaction sig_action;

     sigemptyset(&signal_set);
     sigaddset(&signal_set, SIGCHLD);
     sigaddset(&signal_set, SIGTSTP);
     sigaddset(&signal_set, SIGTTOU);
     sigaddset(&signal_set, SIGTTIN);
     sigaddset(&signal_set, SIGPIPE);
     sigprocmask(SIG_BLOCK, &signal_set, NULL);

     // TODO: implement the below properly and add a signal handler with a static reference or something
     /*
     sig_action.sa_handler = signal_handler;
     sigemptyset(&sig_action.sa_mask);

     sig_action.sa_flags = 0;
     sigaction(SIGHUP,  &sig_action, NULL);
     sigaction(SIGTERM, &sig_action, NULL);
     sigaction(SIGINT,  &sig_action, NULL);
     */
}
