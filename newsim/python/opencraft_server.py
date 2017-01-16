"""
OpenCraft: python_server.py - the twisted/quarry server
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

"""
import sys
import random
from math import floor

import event_types
import boost_log
logger = boost_log.logger()

from cpp_events import cpp_events
cpp_dispatcher = cpp_events()

from twisted.internet import reactor
from quarry.net.server import ServerFactory, ServerProtocol

from quarry.data import packets
from quarry.utils.crypto import Cipher
from quarry.utils.buffer import Buffer, BufferUnderrun
from quarry.utils.errors import ProtocolError
from quarry.utils.tasks import Tasks

class OpenCraftProtocol(ServerProtocol):
   """ On the C++ side, we call this client_conn
   """
   def __init__(self, factory, remote_addr):
       self.factory = factory
       self.remote_addr = remote_addr

       self.buff_type = self.factory.buff_type
       self.recv_buff = self.buff_type()
       self.cipher = Cipher()
       self.tasks = Tasks()

       self.logger = logger

       self.connection_timer = self.tasks.add_delay(
            self.factory.connection_timeout,
            self.connection_timed_out)

       self.setup()
   def player_joined(self):
       self.in_game = True
       self.switch_protocol_mode("play")
       logger.info(str('%s has joined' % self.display_name))
       # TODO - broadcast the event here
   def player_left(self):
       self.in_game = False
       logger.info(str('%s has left' % self.display_name))
       # TODO - broadcast a leave event of some kind here
   def packet_received(self, buff, name):
       buff.save()
       ServerProtocol.packet_received(self,buff,name)
       buff.restore()
       event_id = event_types.event_ids['on_packet_%s' % name]
       cpp_dispatcher.on_packet(self,event_id,str(buff.read()))


class OpenCraftFactory(ServerFactory):
   protocol    = OpenCraftProtocol
   motd        = 'OpenCraft server'
   online_mode = False

class OpenCraftServer:
   def run(self):
       logger.debug('Starting server...')
       proto_factory = OpenCraftFactory()
       logger.info('Server ready!')
       proto_factory.listen('0.0.0.0',25565)
       reactor.run()

if __name__=='__main__':
   server = OpenCraftServer()
   server.run()
