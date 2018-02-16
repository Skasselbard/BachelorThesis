#!/bin/bash
#redirect output
exec > test.out                                                                      
exec 2>&1
#testiteration
for i in 1 2 3 4 5 6 7 8 9 10
do
  #threadcount
  for j in 1 2 3 4 20 50
  do
    for branch in master atomics maraPThread maraIntegration
    do
      #checkout
      git checkout -f $branch
      git pull
      #build
      libtoolize
      autoreconf -i
      $PWD/configure CXXFLAGS='-std=c++11'
      make -j4
      #execute
      if [ $j = 1 ] || [ $j = 4 ]; then
        $PWD/src/lola --json="test-$branch-t$j-b0.json" --check=full --threads=$j ./tests/testfiles/phils1000.lola
      fi
      ./src/lola --json="test-$branch-t$j-b100.json" --check=full --threads=$j --bucketing=100 ./tests/testfiles/phils1000.lola
      if [ $branch = maraPThread ] || [ $branch = maraPThread ]; then
        $PWD/src/lola --json="test-$branch-t$j-b1000.json" --check=full --threads=$j --bucketing=1000 ./tests/testfiles/phils1000.lola
      fi      
    done
  done
#print results to file
grep 'runtime' test*.json > "times$i.txt"
done
