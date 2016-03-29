#!/bin/bash
rm data/config/*.json
rm data/lattice/*.json

python python/config.py
python python/lattice.py

mkdir -p data/evol

nf=0
for conf in data/config/*.json
do
  for latt in data/lattice/*.json
  do
    ./bin/week7 -evolve $conf $latt data/evol/fcc-$nf.json &
    let nf=$nf+1
  done
done
