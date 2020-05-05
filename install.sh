#!/bin/bash


if [[ ! $(sudo echo 0) ]]; then exit; fi

cp -r src/OptFrame /usr/local/include/

mkdir /usr/local/optframe

cp -r Examples /usr/local/optframe/
cp -r src      /usr/local/optframe/
cp -r tests    /usr/local/optframe/
cp    makefile /usr/local/optframe/


