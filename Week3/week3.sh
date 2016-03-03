#!/bin/bash

echo "Generating and evolving rectangular lattice."
./bin/week3 -rect 2 10 1.0 2.0 data/rectangular.json
./bin/week3 -evolve 0.01 1000000 10000 data/rectangular.json data/rectangular-evolution.json

#echo "Generating plots."
#rm report/graphs/*.png
#python python/graphs.py

#echo "Generating MP4."
#rm report/graphs/rectangular-evolution.mp4
#ffmpeg -r 60 -i report/graphs/evolution-%4d.png -vcodec libx264 -vf format=yuv420p report/graphs/rectangular-evolution.mp4

echo "Generating and evolving hexagonal lattice."
python python/lattice.py
./bin/week3 -gen data/lattice-hexa.json data/hexagonal.json
./bin/week3 -evolve 0.1 1000000 10000 data/hexagonal.json data/hexagonal-evolution.json


echo "Generating and evolving FCC lattice."
./bin/week3 -gen data/lattice-fcc.json data/fcc.json
./bin/week3 -evolve 0.1 1000000 10000 data/fcc.json data/fcc-evolution.json

echo "Generating and evolving SC lattice."
./bin/week3 -gen data/lattice-sc.json data/sc.json
./bin/week3 -evolve 0.1 1000000 10000 data/sc.json data/sc-evolution.json

#echo "Generating plots."
#rm report/graphs/*.png
#python python/graphs.py

#echo "Generating MP4."
#rm report/graphs/hexagonal-evolution.mp4
#ffmpeg -r 60 -i report/graphs/evolution-%4d.png -vcodec libx264 -vf format=yuv420p report/graphs/hexagonal-evolution.mp4
