#!/bin/bash

for i in `seq 100`
do
  ./bin/week3 -gen data/lattice-fcc-$i.json data/fcc-$i.json
  ./bin/week3 -evolve 0.1 100000 50000 data/fcc-$i.json data/fcc-evolution-$i.json
  
  ./bin/week3 -gen data/lattice-hexa-$i.json data/hexa-$i.json
  ./bin/week3 -evolve 0.1 100000 50000 data/hexa-$i.json data/hexa-evolution-$i.json
done
