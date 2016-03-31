#!/bin/bash

mkdir -p data/evol2 data/config data/lattice

rm data/config/*.json
rm data/lattice/*.json

python python/config.py
python python/lattice.py

nf=$(($(\ls -afq data/evol | wc -l)-2))
proc=$(($(grep -c ^processor /proc/cpuinfo)+1))
for conf in data/config/*.json
do
  for latt in data/lattice/*.json
  do
    ./bin/week7 -muvt $conf $latt data/evol2/fcc-$nf.json &
    let nf=$nf+1
    while [ $(ps -ef | grep -v grep | grep week7 | wc -l) -gt $proc ]
    do
      sleep 5.0
    done
  done
done
wait
