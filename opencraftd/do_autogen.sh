#!/bin/sh
./genversion.sh
python autogen_packets.py >include/packets.autogen.h
