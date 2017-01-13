""" Autogenerates a header file with packet IDs by using quarry.data - should probably switch this to burger
"""
from quarry.data import packets

DEFAULT_PROTO = 315

print "#pragma once"
print "#define DEFAULT_PROTO %s" % DEFAULT_PROTO

for k,v in packets.packet_idents.items():
    if k[0]==DEFAULT_PROTO:
       print "#define PACKET_ID_%s_%s_%s %s" % (k[1].upper(),k[3].upper(),k[2].upper(),hex(v))
