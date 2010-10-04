#!/bin/bash

clear
echo "Name of project: "
read project
echo "Type of keyA: "
read keyA
echo "Type of A: "
read A
echo "Type of keyB: "
read keyB
echo "Type of B: "
read B
echo "Type of C: "
read C

cp ./.tpl/MRI.tpl ./MRI.cpp

t1="s/\$keyA/$keyA/g"
sed -e $t1 < MRI.cpp > MRI.cpp.2
t2="s/\$A/$A/g"
sed -e $t2 < MRI.cpp.2 > MRI.cpp.3
t3="s/\$keyB/$keyB/g"
sed -e $t3 < MRI.cpp.3 > MRI.cpp.4
t4="s/\$B/$B/g"
sed -e $t4 < MRI.cpp.4 > MRI.cpp.5
t5="s/\$C/$C/g"
sed -e $t5 < MRI.cpp.5 > MRI.cpp.6
t6="s/\$MRI/SeqMR/g"
sed -e $t6 < MRI.cpp.6 > MRI.cpp
rm MRI.cpp.2 MRI.cpp.3 MRI.cpp.4 MRI.cpp.5 MRI.cpp.6
mkdir $project
mv MRI.cpp $project/$project.cpp

echo "Generating file .."
