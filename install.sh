#!/bin/bash

echo "This script requires 'sudo' to install OptFrame"

if [[ ! $(sudo echo 0) ]]; then exit; fi

echo "Installing OptFrame (with OptFCore) into /usr/local/include and /usr/local/optframe"

cp -r src/OptFrame /usr/local/include/
cp -r src/OptFCore /usr/local/include/

mkdir /usr/local/optframe

cp -r Examples /usr/local/optframe/
cp -r src      /usr/local/optframe/
cp -r tests    /usr/local/optframe/
cp    makefile /usr/local/optframe/


