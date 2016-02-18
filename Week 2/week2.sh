#!/bin/bash

echo "Random walk, dim = 2, steps = 1000, w/o bc"
./bin/week2 -walk 2 1000 data/randomwalk2.json
echo "Random walk, dim = 2, steps = 1000, w/ bc, box = 10"
./bin/week2 -pwalk 2 1000 10.0 data/prandomwalk2.json
echo "<R^2>, dim = 2, steps = 1000, walks = 1000, w/o bc"
./bin/week2 -rsquared 2 1000 1000 data/rsquared2.json
echo "<R^2>, dim = 2, steps = 1000, walks = 1000, w/ bc, box = 10"
./bin/week2 -prsquared 2 1000 1000 10.0 data/prsquared2.json

echo "Random walk, dim = 3, steps = 1000, w/o bc"
./bin/week2 -walk 3 1000 data/randomwalk2.json
echo "Random walk, dim = 3, steps = 1000, w/ bc, box = 10"
./bin/week2 -pwalk 3 1000 10.0 data/prandomwalk2.json
echo "<R^2>, dim = 3, steps = 1000, walks = 1000, w/o bc"
./bin/week2 -rsquared 3 1000 1000 data/rsquared2.json
echo "<R^2>, dim = 3, steps = 1000, walks = 1000, w/ bc, box = 10"
./bin/week2 -prsquared 3 1000 1000 10.0 data/prsquared2.json
