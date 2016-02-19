#!/bin/bash

echo "Computing Pi with N = 2^20 values"
./bin/week2 -N $((2**20))
echo ""

echo "Computing Pi with an error less than 0.001"
./bin/week2 -e 0.001
echo ""

echo "Random walks in 2D"
echo "Random walk, dim = 2, steps = 100, w/o bc"
./bin/week2 -walk 2 100 data/randomwalk2.json
echo "Random walk, dim = 2, steps = 100, w/ bc, box = 20"
./bin/week2 -pwalk 2 100 20.0 data/prandomwalk2.json
echo "<R^2>, dim = 2, steps = 100, walks = 10000, w/o bc"
./bin/week2 -rsquared 2 100 10000 data/rsquared2.json
echo "<R^2>, dim = 2, steps = 100, walks = 10000, w/ bc, box = 20"
./bin/week2 -prsquared 2 100 10000 20.0 data/prsquared2.json
echo ""

echo "Random walks in 3D"
echo "Random walk, dim = 3, steps = 100, w/o bc"
./bin/week2 -walk 3 100 data/randomwalk3.json
echo "Random walk, dim = 3, steps = 100, w/ bc, box = 20"
./bin/week2 -pwalk 3 100 20.0 data/prandomwalk3.json
echo "<R^2>, dim = 3, steps = 200, walks = 10000, w/o bc"
./bin/week2 -rsquared 3 200 10000 data/rsquared3.json
echo "<R^2>, dim = 3, steps = 200, walks = 10000, w/ bc, box = 20"
./bin/week2 -prsquared 3 200 10000 20.0 data/prsquared3.json

echo ""
echo "Generating graphs"
python python/graphs.py
