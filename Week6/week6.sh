#!/bin/bash

python python/config.py
python python/lattice.py
./bin/week6 -evolve data/config.json data/test-fcc.json data/evol-fcc.json
