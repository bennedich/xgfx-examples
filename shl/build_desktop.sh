#!/bin/bash

echo "*******************"
echo "** Build Desktop **"
echo "*******************"

rm -rf ./build/desktop/
rm -rf ./bin/desktop/
mkdir ./build/
mkdir ./build/desktop/
cd ./build/desktop/
cmake ../..
make
