#!/bin/bash

#./bin/week3 -rect 2 10 1.0 2.0 data/rectangular.json
#./bin/week3 -evolve 0.01 1000000 500 data/rectangular.json data/rectangular-evolution.json

#echo "Generating plots."
#rm report/graphs/*.png
#python python/graphs.py

#echo "Generating MP4."
#rm report/graphs/rectangular-evolution.mp4
#ffmpeg -r 60 -i report/graphs/evolution-%4d.png -vcodec libx264 -vf format=yuv420p report/graphs/rectangular-evolution.mp4

python python/lattice.py
./bin/week3 -gen data/lattice-hexa.json data/hexagonal.json
./bin/week3 -evolve 0.1 1000000 1000 data/hexagonal.json data/hexagonal-evolution.json

#./bin/week3 -gen data/lattice-fcc.json data/fcc.json
#./bin/week3 -evolve 0.1 300000 1000 data/fcc.json data/fcc-evolution.json

#./bin/week3 -gen data/lattice-sc.json data/sc.json
#./bin/week3 -evolve 0.1 1000000 700 data/sc.json data/sc-evolution.json

echo "Generating plots."
rm report/graphs/*.png
python python/graphs.py

echo "Generating MP4."
rm report/graphs/hexagonal-evolution.mp4
ffmpeg -r 60 -i report/graphs/evolution-%4d.png -vcodec libx264 -vf format=yuv420p report/graphs/hexagonal-evolution.mp4
