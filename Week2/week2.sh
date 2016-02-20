#!/bin/bash

echo "Computing Pi with N = 2^20 points"
./bin/week2 -N $((2**20))
echo ""

echo "Computing Pi (error less than 0.00005)"
./bin/week2 -e 0.00005
echo ""

echo "Random walks in 2D"
echo "Random walk, steps = 10000, w/o bc"
./bin/week2 -walk 2 10000 data/randomwalk2.json
echo "Random walk, steps = 10000, w/ bc, box = 5.0"
./bin/week2 -pwalk 2 10000 5.0 data/prandomwalk2.json
echo "<R^2>, steps = 200, walks = 10000, w/o bc"
./bin/week2 -rsquared 2 200 10000 data/rsquared2.json
echo "<R^2>, steps = 200, walks = 10000, w/ bc, box = 20.0"
./bin/week2 -prsquared 2 200 10000 20.0 data/prsquared2.json
echo ""

echo "Random walks in 3D"
echo "Random walk, steps = 10000, w/o bc"
./bin/week2 -walk 3 10000 data/randomwalk3.json
echo "Random walk, steps = 10000, w/ bc, box = 5.0"
./bin/week2 -pwalk 3 10000 5.0 data/prandomwalk3.json
echo "<R^2>, steps = 200, walks = 10000, w/o bc"
./bin/week2 -rsquared 3 200 10000 data/rsquared3.json
echo "<R^2>, steps = 200, walks = 10000, w/ bc, box = 20.0"
./bin/week2 -prsquared 3 200 10000 20.0 data/prsquared3.json
echo ""
