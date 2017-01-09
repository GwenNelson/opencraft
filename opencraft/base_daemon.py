"""
OpenCraft: base_daemon.py - implementation of the base daemon
Copyright (C) 2016 GarethNelson

This file is part of OpenCraft

OpenCraft is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

OpenCraft is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OpenCraft.  If not, see <http://www.gnu.org/licenses/>.

Note:
  This will almost certainly NOT work on windows - but if you're using windows to host a server you're too incompetent to bother to read this documentation anyway.

"""
import threading

import logging
import logging.handlers
import sys
import atexit
import os
import errno
import signal


class BaseDaemon(object):
   """ The base daemon class from which others inherit

   This class can be used directly in theory, but it makes more sense to inherit from it or use one of the other daemon classes.

   Keyword args:
      pidfile (str): Path to the PID file the daemon will use
      stdin (str):   Path to the stdin device to use
      stdout (str):  Path to the stdout device
      stderr (str):  Path to the stderr device
   """
   def __init__(self,pidfile='./daemon.pid',stdin='/dev/null', stdout='/dev/null', stderr='/dev/null'):
       self.pidfile = os.path.abspath(pidfile)
       self.stdin   = os.path.abspath(stdin)
       self.stdout  = os.path.abspath(stdout)
       self.stderr  = os.path.abspath(stderr)
       self.active  = False
       self.logger  = self.get_logger()

   def get_logger(self):
       """ Sets up the default logger

       This method is used at startup to create a simple logger. The default implementation writes to syslog.

       Returns:
         logging.Logger: the logger
       """
       self.logger = logging.getLogger(sys.argv[0])
       if not self.logger.handlers:
          handler     = logging.handlers.SysLogHandler(address='/dev/log')
          self.logger.addHandler(handler)
       self.logger.setLevel(logging.DEBUG)
       return self.logger

   def handle_rc(self,argv=sys.argv):
       """ Implements rc.d style style commands
       
       To use a daemon in production you probably want a script to put into the init system, that's what this method is for. Simply invoke it and it'll handle start/stop/reload/restart/status.
       
       If the arguments provided are not valid, this method will dump usage information to stdout and then exit.

       Keyword args:
          argv (list): List of command line arguments, generally this should just be sys.argv
       """
       if len(argv)==2:
          cmd = argv[1]
          if cmd == 'start':
             self.start()
          elif cmd == 'stop':
             self.stop()
          elif cmd == 'reload':
             self.do_reload()
          elif cmd == 'restart':
             self.stop()
             self.start()
          elif cmd == 'status':
             if self.is_running():
                print 'Daemon is running with PID %s' % self.get_pid()
             else:
                print 'Daemon is not running'
          else:
            print 'Unknown command!'
            sys.exit(2)
       else:
          print 'Usage: %s start|stop|reload|restart|status' % argv[0]
          sys.exit(2)

   def is_running(self):
       """ Check if the daemon is already running, either in this or in another process
       
       This method will check first if the current process is running the daemon and if not will check the pidfile and check if the PID is still an active process.

       Returns:
          bool: True if the daemon is already running, otherwise False
       """
       if self.active: return True
       if not (self.get_pid() is None): return True
       return False
       
   def get_pid(self):
       """ Get the pid of the active daemon process
       
       If the daemon is running, this will give you the PID, otherwise it'll return None

       Returns:
          int: the PID of the active daemon process or None
       """
       if self.active: return os.getpid()

       pid = None

       try:
          pidfile_fd = open(self.pidfile,'r')
          pid        = int(pidfile_fd.readline().strip('\n'))
          pidfile_fd.close()
       except Exception,e:
          pid = None

       if pid is None: return None
       
       try:
          os.kill(pid,0)
       except OSError,e:
          if e.errno == errno.ESRCH:
             pid = None
          elif e.errno == errno.EPERM:
             pid = None
       return pid

   def fork_me(self):
       """ Used internally - fork me baby, fork me

       Used internally to fork when detaching from parent process, fork me baby, fork me
       """
       try:
          pid = os.fork()
          if pid>0: sys.exit(0)
       except OSError,e:
          sys.stderr.write('Fork failed with error number %s: %s\n' % (e.errno, e.strerror))
          sys.exit(1)

   def pre_stdout(self,pid=0):
       """ Called just before we redirect the file descriptors

       This method is called just before we redirect the file descriptors and make normal output no longer work - use it for announcing startup messages etc.
       In the default implementation this outputs a message announcing the PID of the daemon.

       Warning:
         DO NOT use print to output here, use sys.stderr.write() and then sys.stderr.flush() otherwise bad things will happen.

       Keyword args:
          pid (int): The PID of the daemon process
       """
       sys.stderr.write('Starting daemon with PID %s\n' % pid)
       sys.stderr.flush()

   def atexit_handler(self):
       """ Handle cleanup before closing

       This method is called if the daemon process closes and cleans up things like the PID file. If subclassing, you must make sure to call the parent method here.
       
       Note:
         This method should ONLY do cleanup, after it returns the process dies
       """
       try:
          os.remove(self.pidfile)
       except:
          pass

   def run(self):
       """ Main loop of the daemon process

       This method is where the actual server (or whatever) code goes. The default implementation just runs eventlet.greenthread.sleep(3600) in an infinite loop.
       The default implementation also spits out a log entry once per hour.
       """
       while self.active:
          self.get_logger().info('Default run() is spitting this message out')
          eventlet.greenthread.sleep(3600)

   def handle_hup(self,signum,frame):
       """ Signal handler for SIGHUP

       This method is the default signal handler for SIGHUP. The standard action to take upon a SIGHUP is reloading configuration, so the default implementation calls self.do_reload().

       Args:
         signum (int): Always signal.SIGHUP
         frame: A stack frame, this will be returned to once the handler has finished running
       """
       self.do_reload()

   def do_reload(self):
       """ Reload configuration
       
       This method should be overridden when inheriting from the class. Your implementation should reload any configuration from disk.
       In the default implementation this does nothing.
       """
       pass

   def daemonize(self):
       """ Daemonize the process

       This is where the magic happens, this method is called by start() to daemonize the process and start running.
       You should not call this directly, instead use start().

       Warning:
          This method does not check if the daemon is already running or not, if it is already running then bad things will happen
       """
       self.get_logger().info('Daemonize started')
       self.fork_me() # fork me baby

       os.chdir('/')  # seperate ourselves from the parent
       os.setsid()
       os.umask(0)

       self.fork_me() # fork me harder baby, after this we should be well and truly forked
       
       pid = os.getpid()

       self.pre_stdout(pid=pid) # display any startup messages

       # write to the PID file
       pidfile_fd = file(self.pidfile,'w+')
       pidfile_fd.write('%s\n' % str(pid))
       pidfile_fd.flush()
       pidfile_fd.close()

       # redirect file descriptors
       _stdin   = file(self.stdin,  'r')
       _stdout  = file(self.stdout, 'a+', 0)
       _stderr  = file(self.stderr, 'a+', 0)
       os.dup2(_stdin.fileno(), sys.stdin.fileno())
       os.dup2(_stdout.fileno(), sys.stdout.fileno())
       os.dup2(_stderr.fileno(), sys.stderr.fileno())


       # register atexit handler
       atexit.register(self.atexit_handler)

       # register SIGHUP handler
       signal.signal(signal.SIGHUP, self.handle_hup)

       # mark us as active
       self.active = True

       # run the actual server code or whatever
       self.run()

   def start(self):
       """ Start the daemon up
   
       Call this method to start the daemon, if it already exists this will cause the process to exit with a return value of 1
       """
       pid = self.get_pid()
       if not (pid is None):
          sys.stderr.write('Daemon already running with PID %s\n' % pid)
          sys.exit(1)
       
       self.daemonize()
       self.run()

   def stop(self):
       """ Stop the daemon

       Call this method to stop the daemon (even if it's in another process). If the daemon is not running this method does nothing.
       """
       if not self.is_running(): return
       self.active = False
       daemon_pid = self.get_pid()
       while True:
          try:
             os.kill(daemon_pid, signal.SIGTERM)
             eventlet.greenthread.sleep(1)
          except OSError,e:
             if e.errno == errno.ESRCH:
                if os.path.exists(self.pidfile):
                   os.remove(self.pidfile)
                sys.exit(0)


if __name__=='__main__':
   d = BaseDaemon()
   d.handle_rc()
