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
       chunk_x,rx = divmod(int(floor(self.cur_pos[0])),16)
       chunk_z,rz = divmod(int(floor(self.cur_pos[2])),16)
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
       self.logger            = logger
   def setup_map(self,chunk_w=4,chunk_h=4):
       self.logger.info('Creating map of size %sX%s chunks:',str(chunk_w),str(chunk_h))

       self.logger.info(' [+] Generating blank chunk columns')
       self.chunk_columns={}

       for x in xrange(chunk_w):
           for z in xrange(chunk_h):
               self.chunk_columns[(x,z)] = {}
               for y in xrange(16):
                   self.chunk_columns[(x,z)][y] = {}
                   for rx in xrange(16):
                       for ry in xrange(16):
                           for rz in xrange(16):
                               self.chunk_columns[(x,z)][y][(rx,ry,rz)] = (0,0,[]) # block id, block damage value - air by default
       
       
       
       self.logger.info(' [+] Generating heightmap for %sX%s blocks',str(chunk_w*16),str(chunk_h*16))
       height_map = smpmap.gen_heightmap(chunk_w*16,chunk_h*16)

       self.logger.info(' [+] Building dirt for %sX%s blocks',str(chunk_w*16),str(chunk_h*16))
       max_x = chunk_w*16
       max_z = chunk_h*16
       for x in xrange(max_x):
           for z in xrange(max_z):
               chunk_x,rx = divmod(x,16)
               chunk_z,rz = divmod(z,16)
               for y in xrange(height_map[(x,z)]):
                   chunk_y,ry = divmod(y,16)
                   self.chunk_columns[(chunk_x,chunk_z)][chunk_y][(rx,ry,rz)] = (3,0,[]) # dirt

       self.logger.info('Generated map!')
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

def get_section_data():
    # this is insane
    fd = open('/dev/urandom','rb')
    retval = fd.read(4096)
    fd.close()
    return retval

def get_light_data():
    fd = open('/dev/urandom','rb')
    retval = fd.read(16*16*8)
    fd.close()
    return retval

class OpenCraftProtocol(ServerProtocol):
   def pack_pos(self,pos_x,pos_y,pos_z):
       val = (int(pos_x)  & 0x3FFFFFF) << 38
       val |= (int(pos_y) & 0xFFF) << 26
       val |= (int(pos_z) & 0x3FFFFFF)
       return self.buff_type.pack('Q',val)
   def gen_chunk_data(self,chunk_x,chunk_z):
       self.logger.info('Generate chunk_data packet for %sX%s',str(chunk_x),str(chunk_z))
       retval  = ''
       retval += self.buff_type.pack('i',chunk_x)
       retval += self.buff_type.pack('i',chunk_z)
       retval += self.buff_type.pack('?',True)      # ground up
       retval += self.buff_type.pack_varint(65535)  # mask

       chunk_sections = ''
       for chunk_y in xrange(16):
           chunk_sections += self.buff_type.pack('b',13) # 13 bits per block
           chunk_sections += self.buff_type.pack_varint(0)         # pal length of 0
           chunk_sections += self.buff_type.pack_varint(4096)      # 4096 bytes for the block data
           chunk_sections += get_section_data()             # placeholder for block data
           chunk_sections += get_light_data()               # placeholder for light data

       retval += self.buff_type.pack_varint(len(chunk_sections))
       retval += chunk_sections

       retval += self.buff_type.pack_varint(0) # no entities

       return retval
   def send_chunk_column(self,chunk_x,chunk_z):
       if not self.factory.chunk_data.has_key((chunk_x,chunk_z)):
          self.factory.chunk_data[(chunk_x,chunk_z)] = self.gen_chunk_data(chunk_x,chunk_z)
       self.send_packet("chunk_data",self.factory.chunk_data[(chunk_x,chunk_z)])
   def packet_player_look(self,buff):
       new_yaw   = buff.unpack('f')
       new_pitch = buff.unpack('f')
       try_jump  = buff.unpack('?')
       self.player_data.cur_yaw   = new_yaw
       self.player_data.cur_pitch = new_pitch
   def packet_client_settings(self,buff):
       locale         = buff.unpack_string()
       view_distance  = buff.unpack('b')
       chat_mode      = buff.unpack_varint()
       chat_colors    = buff.unpack('?')
       displayed_skin = buff.unpack('B')
       main_hand      = buff.unpack_varint()
       self.player_data.view_dist = view_distance
       start_chunk = self.player_data.get_cur_chunk()
       self.send_chunk_column(*start_chunk)
   def send_poslook(self,tp_id=0):
       pos_x,pos_y,pos_z = self.player_data.cur_pos
       yaw               = self.player_data.cur_yaw
       pitch             = self.player_data.cur_pitch
       self.send_packet("player_position_and_look",self.buff_type.pack('d',pos_x)+
                                                   self.buff_type.pack('d',pos_y)+
                                                   self.buff_type.pack('d',pos_z)+
                                                   self.buff_type.pack('f',yaw)+
                                                   self.buff_type.pack('f',pitch)+
                                                   self.buff_type.pack('B',0)+
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
   def packet_player_position(self,buff):
       new_x = buff.unpack('d')
       new_y = buff.unpack('d')
       new_z = buff.unpack('d')
       try_jump = buff.unpack('?')
       self.player_data.cur_pos = (new_x,new_y,new_z)
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
       for x in xrange(start_chunk[0]-1,start_chunk[0]+2):
           for y in xrange(start_chunk[1]-1,start_chunk[1]+2):
               self.send_chunk_column(x,y)
       self.send_poslook(tp_id=random.randint(1000,999999))

class OpenCraftFactory(ServerFactory):
   protocol    = OpenCraftProtocol
   motd        = 'OpenCraft server'
   online_mode = False
   log_level   = logging.DEBUG
   chunk_data  = {}

class OpenCraftServer(base_daemon.BaseDaemon):
   def run(self):
       self.logger.debug('Starting server...')
       proto_factory            = OpenCraftFactory()
       proto_factory.logger     = self.logger
       proto_factory.game_state = GameState(logger=self.logger)
       proto_factory.game_state.setup_map()
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
