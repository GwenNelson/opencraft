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
from math import floor

import logging
import base_daemon

import smpmap

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

CHATMODE_ENABLED = 0
CHATMODE_CMDONLY = 1
CHATMODE_HIDDEN  = 2

class PlayerData:
   def __init__(self,display_name=None,player_uuid=None,game_mode=0,dimension=0,difficulty=0,view_dist=4,chatmode=0):
       self.display_name = display_name
       self.uuid         = player_uuid
       self.game_mode    = game_mode
       self.dimension    = dimension
       self.difficulty   = difficulty
       self.ent_id       = 0
       self.view_dist    = view_dist
       self.chatmode     = chatmode
       self.cur_pos      = (0.0,0.0,0.0) # this should be replaced ASAP
       self.cur_yaw      = 0.0
       self.cur_pitch    = 0.0
       self.pending_tps  = set()
   def get_cur_chunk(self):
       rx,chunk_x = divmod(int(floor(self.cur_pos[0])),16)
       rz,chunk_z = divmod(int(floor(self.cur_pos[2])),16)
       return (chunk_x,chunk_z)
   def get_entity_id(self):
       if self.ent_id == 0:
          self.ent_id = random.randint(0,MAX_ENTS)
       return self.ent_id

class GameState:
   def __init__(self,global_mode=GAME_MODE_CREATIVE,global_difficulty=GAME_DIFF_PEACEFUL,spawn_pos=(128.0,63.0,128.0),logger=None):
       self.global_mode       = global_mode
       self.global_difficulty = global_difficulty
       self.players           = set()
       self.existing_ent_ids  = set()
       self.spawn_pos         = spawn_pos
       self.dimensions        = {}
       self.logger            = logger
   def gen_chunk(self,dimension,chunk_x,chunk_z):
       self.logger.info('Generating chunks around: [%s,%s]',str(chunk_x*16),str(chunk_z*16))
       for rx in xrange(-32,32):
           for ry in xrange(-16,16):
               for rz in xrange(-32,32):
                   self.dimensions[dimension].set_block(pos_or_x=rx+chunk_x,y=ry*16,z=rz+chunk_z,block_id=0,meta=0)
   def get_chunk_data(self,dimension,chunk_x,chunk_z):
       """Returns chunk data for the specified chunk
       """
       if not self.dimensions.has_key(dimension): self.dimensions[dimension] = smpmap.Dimension(dimension)
       if not self.dimensions[dimension].columns.has_key((chunk_x,chunk_z)):
          self.gen_chunk(dimension,chunk_x,chunk_z)
       print self.dimensions[dimension].columns.keys()
       return self.dimensions[dimension].columns[(chunk_x,chunk_z)]
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
       player.cur_pos = self.spawn_pos

class OpenCraftProtocol(ServerProtocol):
   def pack_pos(self,pos_x,pos_y,pos_z):
       val = (int(pos_x)  & 0x3FFFFFF) << 38
       val |= (int(pos_y) & 0xFFF) << 26
       val |= (int(pos_z) & 0x3FFFFFF)
       return self.buff_type.pack('Q',val)
   def encode_chunk(self,chunk_data):
       """Encodes a chunk into a packet-ready format
       """
       pass
   def packet_client_settings(self,buff):
       locale         = buff.unpack_string()
       view_distance  = buff.unpack('b')
       chat_mode      = buff.unpack_varint()
       chat_colors    = buff.unpack('?')
       displayed_skin = buff.unpack('B')
       main_hand      = buff.unpack_varint()

       self.player_data.view_dist = view_distance
   def send_poslook(self):
       pos_x,pos_y,pos_z = self.player_data.cur_pos
       yaw               = self.player_data.cur_yaw
       pitch             = self.player_data.cur_pitch
       tp_id             = random.randint(1,9999999)
       self.player_data.pending_tps.add(tp_id)
       self.send_packet("player_position_and_look",self.buff_type.pack('d',pos_x)+
                                                   self.buff_type.pack('d',pos_y)+
                                                   self.buff_type.pack('d',pos_z)+
                                                   self.buff_type.pack('f',yaw)+
                                                   self.buff_type.pack('f',pitch)+
                                                   self.buff_type.pack('b',0)+
                                                   self.buff_type.pack_varint(tp_id))
   def packet_player_position_and_look(self,buff):
       new_x     = buff.unpack('d')
       new_y     = buff.unpack('d')
       new_z     = buff.unpack('d')
       new_yaw   = buff.unpack('f')
       new_pitch = buff.unpack('f')
       try_jump  = buff.unpack('?')
       # TODO - do proper physics checks here to prevent cheaters
       self.player_data.cur_pos   = (new_x,new_y,new_z)
       self.player_data.cur_yaw   = new_yaw
       self.player_data.cur_pitch = new_pitch
       self.send_poslook()
   def packet_teleport_confirm(self,buff):
       self.player_data.pending_tps.discard(buff.unpack_varint())
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
       self.send_packet("spawn_position",self.pack_pos(*self.factory.game_state.spawn_pos))
       start_chunk = self.player_data.get_cur_chunk()
       chunk_data  = self.factory.game_state.get_chunk_data(self.player_data.dimension,start_chunk[0],start_chunk[1])
#       self.send_poslook()

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
       proto_factory.game_state = GameState(logger=self.logger)
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
