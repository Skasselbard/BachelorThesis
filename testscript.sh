#!/bin/bash

git checkout -f master
git pull

libtoolize
autoreconf -i
export CXXFLAGS=-std=c++11
./configure
make -j4

./src/lola --json="test-master-t1-b0.json" --check=full --threads=1 ./tests/testfiles/phils1000.lola
./src/lola --json="test-master-t1-b100.json" --check=full --threads=1 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-master-t2-b100.json" --check=full --threads=2 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-master-t3-b100.json" --check=full --threads=4 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-master-t4-b100.json" --check=full --threads=4 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-master-t20-b100.json" --check=full --threads=20 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-master-t60-b100.json" --check=full --threads=60 --bucketing=100 ./tests/testfiles/phils1000.lola

git checkout -f atomics
git pull

libtoolize
autoreconf -i
export CXXFLAGS=-std=c++11
./configure
make -j4

./src/lola --json="test-atomics-t1-b100.json" --check=full --threads=1 ./tests/testfiles/phils1000.lola
./src/lola --json="test-atomics-t1-b0.json" --check=full --threads=1 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-atomics-t2-b100.json" --check=full --threads=2 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-atomics-t3-b100.json" --check=full --threads=3 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-atomics-t4-b100.json" --check=full --threads=4 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-atomics-t20-b100.json" --check=full --threads=20 --bucketing=100 ./tests/testfiles/phils1000.lola
# ./src/lola --json="test-atomics-t60-b100.json" --check=full --threads=60 --bucketing=100 ./tests/testfiles/phils1000.lola

git checkout -f maraPThread
git pull

libtoolize
autoreconf -i
export CXXFLAGS=-std=c++11
./configure
make -j4

./src/lola --json="test-maraPThread-t1-b0.json" --check=full --threads=1 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraPThread-t1-b100.json" --check=full --threads=1 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraPThread-t2-b100.json" --check=full --threads=2 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraPThread-t3-b100.json" --check=full --threads=3 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraPThread-t4-b100.json" --check=full --threads=4 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraPThread-t20-b100.json" --check=full --threads=20 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraPThread-t60-b100.json" --check=full --threads=60 --bucketing=100 ./tests/testfiles/phils1000.lola

./src/lola --json="test-maraPThread-t1-b1000.json" --check=full --threads=1 --bucketing=1000 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraPThread-t2-b1000.json" --check=full --threads=2 --bucketing=1000 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraPThread-t3-b1000.json" --check=full --threads=3 --bucketing=1000 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraPThread-t4-b1000.json" --check=full --threads=4 --bucketing=1000 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraPThread-t20-b1000.json" --check=full --threads=20 --bucketing=1000 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraPThread-t60-b1000.json" --check=full --threads=60 --bucketing=1000 ./tests/testfiles/phils1000.lola

git checkout -f maraIntegration
git pull

libtoolize
autoreconf -i
export CXXFLAGS=-std=c++11
./configure
make -j4

./src/lola --json="test-maraIntegration-t1-b0.json" --check=full --threads=1 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraIntegration-t1-b100.json" --check=full --threads=1 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraIntegration-t2-b100.json" --check=full --threads=2 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraIntegration-t3-b100.json" --check=full --threads=3 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraIntegration-t4-b100.json" --check=full --threads=4 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraIntegration-t20-b100.json" --check=full --threads=20 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraIntegration-t60-b100.json" --check=full --threads=60 --bucketing=100 ./tests/testfiles/phils1000.lola

./src/lola --json="test-maraIntegration-t1-b1000.json" --check=full --threads=1 --bucketing=1000 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraIntegration-t2-b1000.json" --check=full --threads=2 --bucketing=1000 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraIntegration-t3-b1000.json" --check=full --threads=3 --bucketing=1000 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraIntegration-t4-b1000.json" --check=full --threads=4 --bucketing=1000 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraIntegration-t20-b1000.json" --check=full --threads=20 --bucketing=1000 ./tests/testfiles/phils1000.lola
./src/lola --json="test-maraIntegration-t60-b1000.json" --check=full --threads=60 --bucketing=1000 ./tests/testfiles/phils1000.lola

#print results to file
grep 'runtime' test*.json > times.txt