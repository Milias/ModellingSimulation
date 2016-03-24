#!/bin/bash

python python/config.py
for i in `seq 0 1 24`
do
  for j in `seq 1 1 10`
  do
    ./bin/week6 -evolve data/config-0.5.json data/density/fcc-$i.json data/0.5/evol-fcc-$i-$j.json
    ./bin/week6 -evolve data/config-1.0.json data/density/fcc-$i.json data/1/evol-fcc-$i-$j.json
    ./bin/week6 -evolve data/config-2.0.json data/density/fcc-$i.json data/2/evol-fcc-$i-$j.json
  done
done
