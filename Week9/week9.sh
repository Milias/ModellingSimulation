#!/bin/bash

bin=week9
dim=3

mkdir -p data/evol-$dim data/config data/lattice

rm data/config/*.json
rm data/lattice/*.json
#rm data/evol/*.json

#./python/lattice.py -u 0 20 $dim data/lattice/uniform-20.json
./python/lattice.py -u 0 30 $dim data/lattice/uniform-30.json
./python/lattice.py -u 0 40 $dim data/lattice/uniform-40.json
./python/lattice.py -u 0 50 $dim data/lattice/uniform-50.json
./python/lattice.py -u 0 60 $dim data/lattice/uniform-60.json
#./python/lattice.py -u 0 100 $dim data/lattice/uniform-100.json
./python/config.py

nf=$(($(\ls -afq data/evol-$dim | wc -l)-2))
proc=$(($(grep -c ^processor /proc/cpuinfo)+1))
for conf in data/config/*.json
do
  for latt in data/lattice/*.json
  do
    ./bin/$bin --ising$dim $conf $latt data/evol-$dim/ising-$nf.json &
    let nf=$nf+1
    while [ $(ps -ef | grep -v grep | grep $bin | wc -l) -gt $proc ]
    do
      sleep 1.0
    done
  done
done
wait
