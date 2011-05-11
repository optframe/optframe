// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// Igor Machado - Mario Henrique Perche - Pablo Munhoz - Sabir Ribas
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#!/bin/bash
#######################################################################
# OptFrame - Project Generator MCT - "Make a Compilable Thing!"
# http://optframe.sourceforge.net           
#######################################################################
#
# Use this script to generate a new project (.hpp files and makefile)
#
#######################################################################

clear
echo "================================================"
echo "OptFrame - Project Generator MCT - Version 1.1  "
echo "           \"Make a Compilable Thing!\"           "
echo "    http://sourceforge.net/projects/optframe/   " 
echo "================================================"
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

cp ./mct/RepTest.tpl ./RepTest.cpp

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

if cp ./mct/Representation.tpl $var
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

if cp ./mct/Solution.tpl $var
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

memproject=""
commamproject=""
typeproject=""
initializememory=""
echo "What's your Memory Structure? It is used for fast re-evaluation. (if it is not necessary leave this field empty)"
read mem

if [ -n "$mem" ];
  then

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

  cp ./mct/MemTest.tpl ./MemTest.cpp

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

  typeproject="typedef $mem Mem$project;"
  memproject="Mem$project"
  commamproject=" , Mem$project"
  initializememory=" , * new Mem$project"

else echo "No memory structure!"
     typeproject=""
     memproject=""
     commamproject=""
     initializememory=" , * new int"
fi

echo


## Creating Memory file

var="./$project/Memory.h"
var_tmp=$var".tmp"

if cp ./mct/Memory.tpl $var
then echo "3. Creating Memory File...[ok]"

     tproject="s/\$typeproject/$typeproject/g"  
     sed -e "$tproject" < $var > $var_tmp
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

if cp ./mct/Evaluation.tpl $var
then echo "4. Creating Evaluation File...[ok]"

     mproject="s/\$memproject/$memproject/g"  
     sed -e "$mproject" < $var > $var_tmp
     mv $var_tmp $var

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

if cp ./mct/ProblemInstance.tpl $var
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

epsilon=""

echo -e "\nIs this a MINIMIZATION or MAXIMIZATION problem?"
read minmax

if [ "$minmax" = "MINIMIZATION" ];
  then epsilon="(a < (b - EPSILON_$project));"
  else epsilon="(a > (b + EPSILON_$project));"
fi

var="./$project/Evaluator.hpp"
var_tmp=$var".tmp"

if cp ./mct/Evaluator.tpl $var
then echo "6. Creating Evaluator...[ok]"

     t="s/\$project/$project/g"
     sed -e "$t" < $var > $var_tmp
     mv $var_tmp $var

     t="s/\$epsilon/$epsilon/g"
     sed -e "$t" < $var > $var_tmp
     mv $var_tmp $var

     t="s/\$initializememory/$initializememory/g"
     sed -e "$t" < $var > $var_tmp
     mv $var_tmp $var

     t="s/\$minmax/$minmax/g"
     sed -e "$t" < $var > $var_tmp
     mv $var_tmp $var

     t="s/\$commamproject/$commamproject/g"
     sed -e "$t" < $var > $var_tmp
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

    var="./$project/NSSeq$neighborhood.hpp"
    var_tmp=$var".tmp"

    if cp ./mct/NSSeq.tpl $var
    then echo "7.$i Creating Neighborhood Structure $neighborhood ...[ok]"
         t="s/\$project/$project/g"  
         sed -e "$t" < $var > $var_tmp
         mv $var_tmp $var
         
         t="s/\$neighborhood/$neighborhood/g"  
         sed -e "$t" < $var > $var_tmp
         mv $var_tmp $var

         t="s/\$commamproject/$commamproject/g"  
         sed -e "$t" < $var > $var_tmp
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

    var="./$project/InitialSolution${initialsolution}.hpp"
    var_tmp=$var".tmp"

    if cp ./mct/InitialSolution.tpl $var
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

if cp ./mct/main.tpl $var
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

##############################################
#          makefile
##############################################
var="./makefile"
var_tmp="makefile.tmp"

if cp ./mct/makefile.tpl $var
then echo "makefile file...[ok]"
     
     t="s/\$project/$project/g"  
     sed "$t" < $var > $var_tmp
     mv $var_tmp $var

else echo "make file...[fail]"
     exit
fi

echo
echo "Congratulations! You can use the following command to compile your project:"
echo "g++ main$project.cpp ./OptFrame/Util/Scanner++/Scanner.cpp -lpthread -o main$project"

echo
echo "Goodbye!"
