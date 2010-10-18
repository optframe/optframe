#!/bin/bash
#######################################################################
# OptFrame - Project Generator MCT - "Make a Compilable Thing!"
# http://optframe.sourceforge.net           
#######################################################################
#
# Use this script to generate a new project (.hpp files and makefile)
#
#######################################################################
# Shell Script reference
#
# http://freeos.com/guides/lsst/index.html
#######################################################################

clear
echo "==============================================="
echo "OptFrame Make a Compilable Thing! - Version 0.2" 
echo "  by Igor M., Mario P., Pablo M., and Sabir R. "
echo "==============================================="
echo
echo "1. What's the name of your project? (Step 1 of 8)"
read name
project=$name
project=`echo $project | sed 's/ //g'` #remove white spaces
project=`echo $project | sed 's_/__g'` #remove slashes

echo -e "\nIs \"$project\" a good abbreviation for your project?\nType the abbreviation or leave it blank."
read p2

if [ -n "$p2" ];
then project="$p2"
fi

project=`echo $project | sed 's/ //g'` #remove white spaces
project=`echo $project | sed 's_/__g'` #remove slashes


echo 
echo "Project file will be named $project.h"
echo

##############################################
#  Creating directory
##############################################

if mkdir $project
then
  echo "Creating project directory...[ok]"
else
  echo "Creating project directory...[fail]"
  exit
fi

##############################################
#  Creating files
##############################################

if echo > "$project.h"
then
  echo "Project file: $project.h [ok]"
else
  echo "Project file: $project.h [fail]"
  exit
fi

echo
echo "Creating project \"$name\""

var="#ifndef "$project"_H_
#define "$project"_H_
"
echo "$var" > "$project.h"
echo

##############################################
#          Solution Representation
##############################################

echo "What's your Solution Representation?"
read rep
rep=`echo "$rep" | sed "s/>/ > /g"`
rep=`echo "$rep" | sed "s/</ < /g"`

echo "Do you need any extra include? (ex.: \"xyz.h\" or <vector>)"

include=""
read incl
while [ -n "$incl" ]
do
  include="$include\n#include $incl"
  read incl
done

cp ./.tpl/RepTest.tpl ./RepTest.cpp

t1="s/\$rep/$rep/g"  
t2="s/\$include/$include/g"  

sed -e "$t1" < RepTest.cpp > RepTest.cpp.2
mv RepTest.cpp.2 RepTest.cpp

sed -e "$t2" < RepTest.cpp > RepTest.cpp.2
mv RepTest.cpp.2 RepTest.cpp

if g++ RepTest.cpp -o RepTest
then echo "Solution Representation Test...[ok]"
     rm RepTest.cpp
     rm RepTest
else echo "Solution Representation Test...[fail]"
fi

echo


## Creating Representation file

var="./$project/Representation.h"
var_tmp=$var".tmp"

if cp ./.tpl/Representation.tpl $var
then echo "1. Creating Representation File...[ok]"

     sed -e "$t1" < $var > $var_tmp
     mv $var_tmp $var

     sed -e "$t2" < $var > $var_tmp
     mv $var_tmp $var

     t="s/\$project/$project/g"  
     sed $t < $var > $var_tmp
     mv $var_tmp $var

     echo "#include \"$var\"" >> $project.h
else echo "1. Creating Representation File...[fail]"
     exit
fi


## Creating Solution file

var="./$project/Solution.h"
var_tmp=$var".tmp"

if cp ./.tpl/Solution.tpl $var
then echo "2. Creating Solution File...[ok]"

     t="s/\$project/$project/g"  
     sed $t < $var > $var_tmp
     mv $var_tmp $var

     echo "#include \"$var\"" >> $project.h
else echo "2. Creating Solution File...[fail]"
     exit
fi


##############################################
#          Memory Structure
##############################################

echo "What's your Memory Structure? (for fast re-evaluation)"
read mem
mem=`echo "$mem" | sed "s/>/ > /g"`
mem=`echo "$mem" | sed "s/</ < /g"`

echo "Do you need any extra include? (ex.: \"xyz.h\" or <vector>)"

include=""
read incl
while [ -n "$incl" ]
do
  include="$include\n#include $incl"
  read incl
done

cp ./.tpl/MemTest.tpl ./MemTest.cpp

t1="s/\$mem/$mem/g"  
t2="s/\$include/$include/g"  

sed -e "$t1" < MemTest.cpp > MemTest.cpp.2
mv MemTest.cpp.2 MemTest.cpp

sed -e "$t2" < MemTest.cpp > MemTest.cpp.2
mv MemTest.cpp.2 MemTest.cpp

if g++ MemTest.cpp -o MemTest
then echo "Solution Memory Test...[ok]"
     rm MemTest.cpp
     rm MemTest
else echo "Solution Memory Test...[fail]"
fi

echo


## Creating Memory file

var="./$project/Memory.h"
var_tmp=$var".tmp"

if cp ./.tpl/Memory.tpl $var
then echo "3. Creating Memory File...[ok]"

     sed -e "$t1" < $var > $var_tmp
     mv $var_tmp $var

     sed -e "$t2" < $var > $var_tmp
     mv $var_tmp $var

     t="s/\$project/$project/g"  
     sed $t < $var > $var_tmp
     mv $var_tmp $var

     echo "#include \"$var\"" >> $project.h
else echo "3. Creating Memory File...[fail]"
     exit
fi


## Creating Evaluation file

var="./$project/Evaluation.h"
var_tmp=$var".tmp"

if cp ./.tpl/Evaluation.tpl $var
then echo "4. Creating Evaluation File...[ok]"

     t="s/\$project/$project/g"  
     sed $t < $var > $var_tmp
     mv $var_tmp $var

     echo "#include \"$var\"" >> $project.h
else echo "4. Creating Evaluation File...[fail]"
     exit
fi



##############################################
#             Problem Instance
##############################################

var="./$project/ProblemInstance.hpp"
var_tmp=$var".tmp"

if cp ./.tpl/ProblemInstance.tpl $var
then echo "5. Creating Problem Instance...[ok]"
     t="s/\$project/$project/g"  
     sed $t < $var > $var_tmp
     mv $var_tmp $var
     echo "#include \"$var\"" >> $project.h
else echo "5. Creating Problem Instance...[fail]"
     exit
fi


##############################################
#             Problem Instance
##############################################

var="./$project/ProblemInstance.hpp"
var_tmp=$var".tmp"

if cp ./.tpl/ProblemInstance.tpl $var
then echo "5. Creating Problem Instance...[ok]"
     t="s/\$project/$project/g"  
     sed $t < $var > $var_tmp
     mv $var_tmp $var
     echo "#include \"$var\"" >> $project.h
else echo "5. Creating Problem Instance...[fail]"
     exit
fi


##############################################
#             Evaluator
##############################################

var="./$project/Evaluator.hpp"
var_tmp=$var".tmp"

if cp ./.tpl/Evaluator.tpl $var
then echo "6. Creating Evaluator...[ok]"
     t="s/\$project/$project/g"  
     sed $t < $var > $var_tmp
     mv $var_tmp $var
     echo "#include \"$var\"" >> $project.h
else echo "6. Creating Evaluator...[fail]"
     exit
fi


##############################################
#               Neighborhood
##############################################

echo -e "\nHow many Neighborhood Structures will be there in your project?"
read nbNS

for ((i=1; i<=$nbNS; i++ ))
do

    if [ $i -eq 1 ];
    then echo "Type the name of your 1st Neighborhood Structure:"
    elif [ $i -eq 2 ];
    then echo "Type the name of your 2nd Neighborhood Structure:"
    elif [ $i -eq 3 ];
    then echo "Type the name of your 3rd Neighborhood Structure:"
    else echo "Type the name of your ${x}th Neighborhood Structure:"
    fi

    read neighborhood

    var="./$project/NSEnum$neighborhood.hpp"
    var_tmp=$var".tmp"

    if cp ./.tpl/NSEnum.tpl $var
    then echo "7.$i Creating Neighborhood Structure $neighborhood ...[ok]"
         t="s/\$project/$project/g"  
         sed $t < $var > $var_tmp
         mv $var_tmp $var
         
         t="s/\$neighborhood/$neighborhood/g"  
         sed $t < $var > $var_tmp
         mv $var_tmp $var
         
         echo "#include \"$var\"" >> $project.h
    else echo "7.$i Creating Neighborhood Structure $neighborhood ...[fail]"
         exit
    fi
done


##############################################
#             Initial Solution
##############################################

echo -e "\nHow many Initial Solution Generators will be there in your project?"
read nbISG

for ((i=1; i<=$nbISG; i++ ))
do

    if [ $i -eq 1 ];
    then echo "Type the name of your 1st Initial Solution Generator:"
    elif [ $i -eq 2 ];
    then echo "Type the name of your 2nd Initial Solution Generator:"
    elif [ $i -eq 3 ];
    then echo "Type the name of your 3rd Initial Solution Generator:"
    else echo "Type the name of your ${x}th Initial Solution Generator:"
    fi

    read initialsolution

    var="./$project/${initialsolution}InitialSolution.hpp"
    var_tmp=$var".tmp"

    if cp ./.tpl/InitialSolution.tpl $var
    then echo "8.$i Creating Initial Solution Generator $initialsolution ...[ok]"
         t="s/\$project/$project/g"  
         sed $t < $var > $var_tmp
         mv $var_tmp $var
         
         t="s/\$initialsolution/$initialsolution/g"  
         sed $t < $var > $var_tmp
         mv $var_tmp $var
         
         echo "#include \"$var\"" >> $project.h
    else echo "8.$i Creating Initial Solution Generator $initialsolution ...[fail]"
         exit
    fi
done

# Closing project file

echo "#endif /*${project}_H_*/" >> "$project.h"



##############################################
#             Main file
##############################################

var="./main$project.cpp"
var_tmp=$var".tmp"

if cp ./.tpl/main.tpl $var
then echo "Main file...[ok]"
     
     t="s/\$project/$project/g"  
     sed "$t" < $var > $var_tmp
     mv $var_tmp $var

     t="s/\$initialsolution/$initialsolution/g"  
     sed "$t" < $var > $var_tmp
     mv $var_tmp $var

     t="s/\$name/$name/g"  
     sed "$t" < $var > $var_tmp
     mv $var_tmp $var

else echo "Main file...[fail]"
     exit
fi


echo
echo "Goodbye!"
