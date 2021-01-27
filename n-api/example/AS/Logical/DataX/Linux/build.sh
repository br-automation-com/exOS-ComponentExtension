#!/bin/sh

finalize() {
    cd ..
    rm -rf build/*
    rm -r build
    sync
    exit $1
}

mkdir build > /dev/null 2>&1
rm -rf build/*

cd build

cmake ..
if [ "$?" -ne 0 ] ; then
    finalize 1
fi

make
if [ "$?" -ne 0 ] ; then
    finalize 2
fi

cpack
if [ "$?" -ne 0 ] ; then
    finalize 3
fi

cp -f exos-comp-*.deb ..

finalize 0
