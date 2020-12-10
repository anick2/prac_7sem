#!/bin/bash

touch ./results/series$1.csv
cd cmake-build-release
echo 'result,user,sys' 1> ../results/series$1.csv
for ((j = 0; j < 10; j++)); do
  gtime -f '%U, %S' -ao ../results/series$1.csv ./genalg $1 $j
done
