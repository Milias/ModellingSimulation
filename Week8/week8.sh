#!/bin/bash

rm -rf data/ozsolver-*.data

nf=1
for eta in 0.2 0.3 0.4
do
  ./bin/week8 -oz 10.0 $eta 12 0.000000001 data/ozsolver-$nf.data
  let nf=nf+1
done
