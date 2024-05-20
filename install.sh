#!/bin/bash

echo "This script requires 'sudo' to install OptFrame"

if [[ ! $(sudo echo 0) ]]; then exit; fi

echo "Installing OptFrame (with OptFCore) into /usr/local/include and /usr/local/optframe"

read -r -p "Continue? y/n" input

case $input in
    [yY][eE][sS]|[yY])
    echo "Yes"

    cp -r include/OptFrame /usr/local/include/
    cp -r include/OptFCore /usr/local/include/

    mkdir -p /usr/local/optframe

    cp -r Examples /usr/local/optframe/
    cp -r include  /usr/local/optframe/
    cp -r src      /usr/local/optframe/
    cp -r tests    /usr/local/optframe/
    cp    makefile /usr/local/optframe/
    echo "Finished installation!"
 ;;
    [nN][oO]|[nN])
    echo "No"
    echo "Aborting..."
    exit 1
       ;;
    *)
 echo "Invalid input!"
 echo "Aborting..."
 exit 1
 ;;
esac



