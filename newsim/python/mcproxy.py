"""
OpenCraft: mcproxy.py - a proxy that dumps packets for debugging
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
along with python-sock-tools.  If not, see <http://www.gnu.org/licenses/>.

This should be simple enough to use, it binds to port 25566 and then connects to localhost:25565.

Packets are then dumped to stdout.

"""
import sys
import random

import logging
import base_daemon

from twisted.internet    import reactor
from twisted.internet import reactor, protocol, defer
from twisted.python import failure
from quarry.net.protocol import Factory
from quarry.net.protocol import Protocol

class MCProxyClientProtocol(Protocol):
   recv_direction = "downstream"
   send_direction = "upstream"
   def setup(self):
       self.factory.listener_protocol.client = self

   def packet_received(self, buff, name):
       self.logger.debug("S>C %s",name)
       self.listener_protocol.send_packet(name,buff.read())
       self.factory.listener_protocol.client = self

   def connection_made(self):
       self.factory.listener_protocol.client = self

class MCProxyClientFactory(Factory, protocol.ClientFactory):
   protocol          = MCProxyClientProtocol
   listener_protocol = None
   log_level         = logging.DEBUG
   def connect(self, host, port=25565):
       reactor.connectTCP(host, port, self, self.connection_timeout)
       return defer.succeed(self)

class MCProxyListenerProtocol(Protocol):
   recv_direction = "upstream"
   send_direction = "downstream"
   client         = None   

   def connection_made(self):
       self.logger.debug("Connection made")
       new_client_factory                   = self.factory.client_factory()
       new_client_factory.logger            = self.factory.logger
       new_client_factory.listener_protocol = self
       new_client_factory.connect('127.0.0.1',25565)
   def packet_received(self, buff, name):
       while self.client is None: 
          d = defer.Deferred()
          reactor.callLater(0.1, d.callback, None)
          yield d

       self.logger.debug("C>S %s",name)
       self.client.send_packet(name,buff.read())


class MCProxyListenerFactory(Factory):
   protocol       = MCProxyListenerProtocol
   client_factory = MCProxyClientFactory
   log_level      = logging.DEBUG
   def listen(self, host, port=25565):
       reactor.listenTCP(port, self, interface=host)

class MCProxyServer(base_daemon.BaseDaemon):
   def run(self):
       self.logger.debug('Starting server...')

       proto_factory              = MCProxyListenerFactory()
       proto_factory.logger       = self.logger

       self.logger.info('Server ready!')
       proto_factory.listen('0.0.0.0',25566)
       reactor.run()

if __name__=='__main__':
   server = MCProxyServer()
   if len(sys.argv)==2:
      if sys.argv[1]=='-d':
         server.run()
      else:
         server.handle_rc()
   else:
     server.handle_rc()
