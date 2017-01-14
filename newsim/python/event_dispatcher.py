"""
OpenCraft: event_dispatcher.py - the python side of the event dispatcher
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

This module provides the python side of the event dispatcher described in the design document.
It is NOT thread safe unless appropriately wrapped. For the sake of a future saner design, fire_event() does not actually run the handlers.
Instead, use fire_event() (possibly multiple times if you so choose) and then run_events().
"""

import queue

class event_dispatcher:
   def __init__(self):
       self.event_handlers = {}
       self.pending_events = queue.Queue(0)
   def add_handler(self,event_id,handler):
       """ add a new handler for the specified event type
       """
       if not self.event_handlers.has_key(event_id):
          self.event_handlers[event_id] = set()
       self.event_handlers[event_id].add(handler)
   def fire_event(self,event_id,event_data):
       """ send an event - this will be placed on the queue and then run ASAP
       """
       self.pending_events.put((event_id,event_data))
   def run_events(self):
       """ run all pending events
       """
       while not self.pending_events.empty():
          if not self.pending_events.empty():
             event = None
             try:
                event = self.pending_events.get_nowait()
             except:
                pass
             if self.event_handlers.has_key(event[0]):
                for handler in self.event_handlers[event[0]]:
                    try:
                       params = event[1]
                       handler(*params)
                    except:
                       pass
