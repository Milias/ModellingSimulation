#!/bin/bash
rm data/config/*.json
rm data/lattice/*.json

python python/config.py
python python/lattice.py

mkdir -p data/evol

nf=0
proc=12
q_jobs=0
for conf in data/config/*.json
do
  for latt in data/lattice/*.json
  do
    ./bin/week7 -evolve $conf $latt data/evol/fcc-$nf.json &
    let nf=$nf+1
    let q_jobs=$q_jobs+1
    if [ $q_jobs -eq $proc ]
    then
      echo "$q_jobs queried"
      let q_jobs=0
      wait
    fi
  done
done
wait
