#!/bin/bash

bin=week10
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
