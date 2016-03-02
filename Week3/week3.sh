#!/bin/bash

#./bin/week3 -rect 2 10 1.0 data/rect2a.json
#./bin/week3 -hex 10 1.0 data/hexa.json
#./bin/week3 -rect 2 10 1.0 2.0 data/rect2b.json
#./bin/week3 -hex 10 1.0 2.0 data/hexb.json

#./bin/week3 -evolve 0.01 1000 10 data/rect2a.json data/rect2a-evolution.json

python python/lattice.py
./bin/week3 -gen data/lattice-hexa.json data/hexagonal.json
./bin/week3 -evolve 0.1 1000000 10000 data/hexagonal.json data/hexagonal-evolution.json

#./bin/week3 -fcc 3 10 1.0 2.5 data/fcc.json
#./bin/week3 -evolve 0.1 100000 100 data/fcc.json data/fcc-evolution.json

echo "Generating plots."
rm report/graphs/*.png
python python/graphs.py

echo "Generating MP4."
rm report/graphs/hexagonal-evolution.mp4
ffmpeg -r 30 -i report/graphs/evolution-%4d.png -vcodec libx264 -vf format=yuv420p report/graphs/hexagonal-evolution.mp4
