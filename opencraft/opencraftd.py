"""
OpenCraft: OpenCraftd.py - Entry point for the server daemon
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

This is the entry point for opencraftd
"""
import sys
import random

import logging
import base_daemon

from twisted.internet import reactor
from quarry.net.server import ServerFactory, ServerProtocol

GAME_MODE_SURVIVAL  = 0
GAME_MODE_CREATIVE  = 1
GAME_MODE_ADVENTURE = 2
GAME_MODE_SPECTATOR = 3

GAME_DIMENSION_NETHER    = -1
GAME_DIMENSION_OVERWORLD =  0
GAME_DIMENSION_END       =  1

GAME_DIFF_PEACEFUL = 0
GAME_DIFF_EASY     = 1
GAME_DIFF_NORMAL   = 2
GAME_DIFF_HARD     = 3

MAX_ENTS = 999999

class PlayerData:
   def __init__(self,display_name=None,player_uuid=None,game_mode=0,dimension=0,difficulty=0):
       self.display_name = display_name
       self.uuid         = player_uuid
       self.game_mode    = game_mode
       self.dimension    = dimension
       self.difficulty   = difficulty
       self.ent_id       = 0
   def get_entity_id(self):
       if self.ent_id == 0:
          self.ent_id = random.randint(0,MAX_ENTS)
       return self.ent_id

class GameState:
   def __init__(self,global_mode=GAME_MODE_CREATIVE,global_difficulty=GAME_DIFF_PEACEFUL):
       self.global_mode       = global_mode
       self.global_difficulty = global_difficulty
       self.players           = set()
       self.existing_ent_ids  = set()
   def add_player(self,player):
       """Add a new player to the game state

       This method will also check the entity ID and ensure it doesn't clash, updating if needed

       Args:
          player (PlayerData): The player to add
       """
       player_ent_id = player.get_entity_id()
       if player_ent_id in self.existing_ent_ids:
          while player.get_entity_id() in self.existing_ent_ids:
             player.ent_id = 0
       self.existing_ent_ids.add(player.get_entity_id())
       self.players.add(player)

class OpenCraftProtocol(ServerProtocol):
   def player_joined(self):
       ServerProtocol.player_joined(self)
       self.factory.logger.info('%s has joined' % self.display_name)
       self.player_data = PlayerData(display_name= self.display_name,
                                     player_uuid = self.uuid,
                                     game_mode   = self.factory.game_state.global_mode,
                                     dimension   = GAME_DIMENSION_OVERWORLD,
                                     difficulty  = self.factory.game_state.global_difficulty)
       self.factory.game_state.add_player(self.player_data)
       self.send_packet("join_game",self.buff_type.pack('i',self.player_data.get_entity_id())+
                                    self.buff_type.pack('B',self.player_data.game_mode)+
                                    self.buff_type.pack('i',self.player_data.dimension)+
                                    self.buff_type.pack('B',self.player_data.difficulty)+
                                    self.buff_type.pack('B',100)+
                                    self.buff_type.pack_string('default')+
                                    self.buff_type.pack('?',True))

class OpenCraftFactory(ServerFactory):
   protocol    = OpenCraftProtocol
   motd        = 'OpenCraft server'
   online_mode = False
   log_level   = logging.DEBUG

class OpenCraftServer(base_daemon.BaseDaemon):
   def run(self):
       self.logger.debug('Starting server...')
       proto_factory            = OpenCraftFactory()
       proto_factory.logger     = self.logger
       proto_factory.game_state = GameState()
       proto_factory.listen('0.0.0.0',25565)
       reactor.run()

if __name__=='__main__':
   server = OpenCraftServer()
   if len(sys.argv)==2:
      if sys.argv[1]=='-d':
         server.run()
      else:
         server.handle_rc()
   else:
     server.handle_rc()
