#!/bin/bash

echo "***************"
echo "** Build Web **"
echo "***************"

rm -rf ./build/web/
rm -rf ./bin/web/
mkdir ./build/
mkdir ./build/web/
cd ./build/web/
emcmake cmake $1 ../..
emmake make $1
