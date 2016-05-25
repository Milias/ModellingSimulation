#!/bin/bash

rm data/config/*
rm data/init/*
rm data/evol/*

./python/config.py
./python/lattice.py

bin=week10
nf=$(($(\ls -afq data/evol | wc -l)-2))
proc=$(($(grep -c ^processor /proc/cpuinfo)+1))
for conf in data/config/*.json
do
  for latt in data/init/*.json
  do
    ./bin/$bin --elevator $conf $latt data/evol/elevator-$nf.json &
    let nf=$nf+1
    while [ $(ps -ef | grep -v grep | grep $bin | wc -l) -gt $proc ]
    do
      sleep 1.0
    done
  done
done
wait
