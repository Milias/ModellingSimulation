#!/bin/bash

bin=week9

mkdir -p data/evol data/config data/lattice

rm data/config/*.json
rm data/lattice/*.json

./python/lattice.py -u 1 1024 1024 data/lattice/uniform.json
#./python/lattice.py -r 1 64 64 data/lattice/random.json
./python/config.py

nf=$(($(\ls -afq data/evol | wc -l)-2))
proc=$(($(grep -c ^processor /proc/cpuinfo)+1))
for conf in data/config/*.json
do
  for latt in data/lattice/*.json
  do
    ./bin/$bin --ising $conf $latt data/evol/ising-$nf.json &
    let nf=$nf+1
    while [ $(ps -ef | grep -v grep | grep $bin | wc -l) -gt $proc ]
    do
      sleep 5.0
    done
  done
done
wait
