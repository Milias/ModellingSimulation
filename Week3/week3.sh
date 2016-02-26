#!/bin/bash

#./bin/week3 -rect 2 10 1.0 data/rect2a.json
#./bin/week3 -hex 10 1.0 data/hexa.json
#./bin/week3 -rect 2 10 1.0 2.0 data/rect2b.json
#./bin/week3 -hex 10 1.0 2.0 data/hexb.json

#./bin/week3 -evolve 0.01 1000 10 data/rect2a.json data/rect2a-evolution.json

python python/lattice.py
./bin/week3 -gen data/lattice-hexa.json data/hexagonal.json
./bin/week3 -evolve 0.1 1000000 100000 data/hexagonal.json data/hexagonal-evolution.json

echo ""
echo "Generating plots."
python python/graphs.py
