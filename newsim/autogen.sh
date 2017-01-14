#!/bin/sh
./genversion.sh
python python/gen_events_header.py >include/event_types.autogen.h
