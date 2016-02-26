#!/bin/bash

./bin/week3 -rect 2 10 1.0 data/rect2a.json
./bin/week3 -hex 10 1.0 data/hexa.json
./bin/week3 -rect 2 10 1.0 2.0 data/rect2b.json
./bin/week3 -hex 10 1.0 2.0 data/hexb.json

echo ""
echo "Generating plots."
python python/graphs.py
