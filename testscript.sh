#!/bin/bash

git stash
git checkout master
git pull

libtoolize
autoreconf -i
export CXXFLAGS=-std=c++11
./configure
make -j4

./src/lola --json="test-master-t1-b100.json" --check=full --threads=1 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-master-t5-b100.json" --check=full --threads=5 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-master-t20-b100.json" --check=full --threads=20 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-master-t60-b100.json" --check=full --threads=60 --bucketing=100 ./tests/testfiles/phils1000.lola

git stash
git checkout atomics
git pull

libtoolize
autoreconf -i
export CXXFLAGS=-std=c++11
./configure
make -j4

./src/lola --json="test-atomics-t1-b100.json" --check=full --threads=1 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-atomics-t5-b100.json" --check=full --threads=5 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-atomics-t20-b100.json" --check=full --threads=20 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-atomics-t60-b100.json" --check=full --threads=60 --bucketing=100 ./tests/testfiles/phils1000.lola

git stash
git checkout maraIntegration
git pull

libtoolize
autoreconf -i
export CXXFLAGS=-std=c++11
./configure
make -j4

./src/lola --json="test-mara-t1-b100.json" --check=full --threads=1 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-mara-t5-b100.json" --check=full --threads=5 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-mara-t20-b100.json" --check=full --threads=20 --bucketing=100 ./tests/testfiles/phils1000.lola
./src/lola --json="test-mara-t60-b100.json" --check=full --threads=60 --bucketing=100 ./tests/testfiles/phils1000.lola

./src/lola --json="test-mara-t1-b1000.json" --check=full --threads=1 --bucketing=1000 ./tests/testfiles/phils1000.lola
./src/lola --json="test-mara-t5-b1000.json" --check=full --threads=5 --bucketing=1000 ./tests/testfiles/phils1000.lola
./src/lola --json="test-mara-t20-b1000.json" --check=full --threads=20 --bucketing=1000 ./tests/testfiles/phils1000.lola
./src/lola --json="test-mara-t60-b1000.json" --check=full --threads=60 --bucketing=1000 ./tests/testfiles/phils1000.lola