#!/bin/sh
./genversion.sh
python gen_load_blocks.py >include/loadblocks.inc

