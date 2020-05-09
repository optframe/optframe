#!/bin/bash
#######################################################################
# OptFrame - Project Generator MCT - "Make a Compilable Thing!"
# Copyright (C) 2009-2017
# http://optframe.sourceforge.net           
#######################################################################
#
# Use this script to generate a new project (.h, .cpp files and makefile)
#
#######################################################################

clear
echo "===================================================="
echo "OptFrame development version - Project Generator MCT"
echo "             \"Make a Compilable Thing!\"           "
echo "      http://sourceforge.net/projects/optframe/     " 
echo "===================================================="
echo
# verify if mct directory content is available
ls mct/RepTest.tpl > /dev/null 2>&1
if [ $? -ne 0  ]; then
   echo "This script should be run on ./bin folder"
   exit 1
fi
# good to go
echo "1. What's the name of your project? (Step 1 of 9)"
read name
project=$name
project=`echo $project | sed 's/ //g'` #remove white spaces
project=`echo $project | sed 's_/__g'` #remove slashes
workdir=../MyProjects

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
#  Creating workdirectory
##############################################

# create (and recreate) projects directory
mkdir -p $workdir
#if mkdir $workdir
#then
#  echo "Creating workdir...[ok]"
#else
#  echo "Creating workdir...[fail]"
#  exit
#fi

##############################################
#  Creating project directory
##############################################

if mkdir $workdir/$project
then
  echo "Creating project directory...[ok]"
else
  echo "Creating project directory...[fail]"
  exit
fi

if mkdir $workdir/$project/src
then
  echo "Creating $workdir/$project/src directory...[ok]"
else
  echo "Creating $workdir/$project/src directory...[fail]"
  exit
fi

if mkdir $workdir/$project/tests
then
  echo "Creating $workdir/$project/tests directory...[ok]"
else
  echo "Creating $workdir/$project/tests directory...[fail]"
  exit
fi

if mkdir $workdir/$project/build
then
  echo "Creating $workdir/$project/build directory...[ok]"
else
  echo "Creating $workdir/$project/build directory...[fail]"
  exit
fi


##############################################
#  Creating files
##############################################

if echo > "$workdir/$project/src/$project.h"
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
echo "$var" > "$workdir/$project/src/$project.h"
echo


##############################################
#          Solution Representation
##############################################

echo "The Representation is the data structure that defines/implements the Solution Space."
echo "What's your Representation?"
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

cp ./mct/RepTest.tpl $workdir/RepTest.cpp

t1="s/\$rep/$rep/g"  
t2="s/\$include/$include/g"  

sed -e "$t1" < $workdir/RepTest.cpp > $workdir/RepTest.cpp.2
mv $workdir/RepTest.cpp.2 $workdir/RepTest.cpp

sed -e "$t2" < $workdir/RepTest.cpp > $workdir/RepTest.cpp.2
mv $workdir/RepTest.cpp.2 $workdir/RepTest.cpp

if g++ $workdir/RepTest.cpp -o $workdir/RepTest
then echo "Solution Representation Test...[ok]"
     rm $workdir/RepTest.cpp
     rm $workdir/RepTest
else echo "Solution Representation Test...[fail]"
fi

echo


## Creating Representation file


var_inc="Representation.h"
var="$workdir/$project/src/Representation.h"
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

     echo "#include \"$var_inc\"" >> $workdir/$project/src/$project.h
else echo "1. Creating Representation File...[fail]"
     exit
fi


##############################################
#          Delta Structure
##############################################
echo "2-3-4. Delta Structure and Solution files are deprecated, use the Solution ADS instead. Moving to next question..."


##############################################
#             Problem Instance
##############################################

var_inc="ProblemInstance.h"
var="$workdir/$project/src/ProblemInstance.h"
var_tmp=$var".tmp"

if cp ./mct/ProblemInstance.tpl $var
then echo "5. Creating Problem Instance...[ok]"
     t="s/\$project/$project/g"  
     sed $t < $var > $var_tmp
     mv $var_tmp $var
     echo "#include \"$var_inc\"" >> $workdir/$project/src/$project.h
else echo "5. Creating Problem Instance...[fail]"
     exit
fi

var_inc="ProblemInstance.cpp"
var="$workdir/$project/src/ProblemInstance.cpp"
var_tmp=$var".tmp"

if cp ./mct/ProblemInstanceCpp.tpl $var
then echo "5. Creating Problem Instance (CPP)...[ok]"
     t="s/\$project/$project/g"  
     sed $t < $var > $var_tmp
     mv $var_tmp $var
else echo "5. Creating Problem Instance (CPP)...[fail]"
     exit
fi


##############################################
#             Evaluator
##############################################

epsilon=""
isMin=""

echo -e "\nIs this a MINIMIZATION or MAXIMIZATION problem?"
read minmax

if [ "$minmax" = "MINIMIZATION" ];
  then epsilon="(f1 < (f2 - EPSILON_$project));"
       isMin="true;"
  else epsilon="(f1 > (f2 + EPSILON_$project));"
       isMin="false;"
fi

var_inc="Evaluator.h"
var="$workdir/$project/src/Evaluator.h"
var_tmp=$var".tmp"

if cp ./mct/Evaluator.tpl $var
then echo "6. Creating Evaluator...[ok]"

     t="s/\$project/$project/g"
     sed -e "$t" < $var > $var_tmp
     mv $var_tmp $var

     t="s/\$epsilon/$epsilon/g"
     sed -e "$t" < $var > $var_tmp
     mv $var_tmp $var

     t="s/\$isMin/$isMin/g"
     sed -e "$t" < $var > $var_tmp
     mv $var_tmp $var

     t="s/\$minmax/$minmax/g"
     sed -e "$t" < $var > $var_tmp
     mv $var_tmp $var

     echo "#include \"$var_inc\"" >> $workdir/$project/src/$project.h
else echo "6. Creating Evaluator...[fail]"
     exit
fi

var_inc="Evaluator.cpp"
var="$workdir/$project/src/Evaluator.cpp"
var_tmp=$var".tmp"

if cp ./mct/EvaluatorCpp.tpl $var
then echo "6. Creating Evaluator (CPP)...[ok]"

     t="s/\$project/$project/g"
     sed -e "$t" < $var > $var_tmp
     mv $var_tmp $var

     t="s/\$epsilon/$epsilon/g"
     sed -e "$t" < $var > $var_tmp
     mv $var_tmp $var

     t="s/\$isMin/$isMin/g"
     sed -e "$t" < $var > $var_tmp
     mv $var_tmp $var

     t="s/\$minmax/$minmax/g"
     sed -e "$t" < $var > $var_tmp
     mv $var_tmp $var

else echo "6. Creating Evaluator (CPP)...[fail]"
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

    var_inc="NSSeq$neighborhood.h"
    var="$workdir/$project/src/NSSeq$neighborhood.h"
    var_tmp=$var".tmp"

    if cp ./mct/NSSeq.tpl $var
    then echo "7.$i Creating Neighborhood Structure $neighborhood ...[ok]"
         t="s/\$project/$project/g"  
         sed -e "$t" < $var > $var_tmp
         mv $var_tmp $var
         
         t="s/\$neighborhood/$neighborhood/g"  
         sed -e "$t" < $var > $var_tmp
         mv $var_tmp $var
         
         echo "#include \"$var_inc\"" >> $workdir/$project/src/$project.h
    else echo "7.$i Creating Neighborhood Structure $neighborhood ...[fail]"
         exit
    fi
    
    var_inc="NSSeq$neighborhood.cpp"
    var="$workdir/$project/src/NSSeq$neighborhood.cpp"
    var_tmp=$var".tmp"

    if cp ./mct/NSSeqCpp.tpl $var
    then echo "7.$i Creating Neighborhood Structure $neighborhood (CPP) ...[ok]"
         t="s/\$project/$project/g"  
         sed -e "$t" < $var > $var_tmp
         mv $var_tmp $var
         
         t="s/\$neighborhood/$neighborhood/g"  
         sed -e "$t" < $var > $var_tmp
         mv $var_tmp $var
         
    else echo "7.$i Creating Neighborhood Structure $neighborhood (CPP) ...[fail]"
         exit
    fi
    
done


##############################################
#             Constructives
##############################################

echo -e "\nHow many Constructive methods will be there in your project?"
read nbISG

for ((i=1; i<=$nbISG; i++ ))
do

    if [ $i -eq 1 ];
    then echo "Type the name of your 1st Constructive:"
    elif [ $i -eq 2 ];
    then echo "Type the name of your 2nd Constructive:"
    elif [ $i -eq 3 ];
    then echo "Type the name of your 3rd Constructive:"
    else echo "Type the name of your ${x}th Constructive:"
    fi

    read constructive

    var_inc="Constructive${constructive}.h"
    var="$workdir/$project/src/Constructive${constructive}.h"
    var_tmp=$var".tmp"

    if cp ./mct/Constructive.tpl $var
    then echo "8.$i Creating Constructive $constructive ...[ok]"
         t="s/\$project/$project/g"  
         sed $t < $var > $var_tmp
         mv $var_tmp $var
         
         t="s/\$constructive/$constructive/g"  
         sed $t < $var > $var_tmp
         mv $var_tmp $var
         
         echo "#include \"$var_inc\"" >> $workdir/$project/src/$project.h
    else echo "8.$i Creating Constructive $constructive ...[fail]"
         exit
    fi

    var_inc="./$project/Constructive${constructive}.cpp"
    var="$workdir/$project/src/Constructive${constructive}.cpp"
    var_tmp=$var".tmp"

    if cp ./mct/ConstructiveCpp.tpl $var
    then echo "8.$i Creating Constructive $constructive (CPP)...[ok]"
         t="s/\$project/$project/g"  
         sed $t < $var > $var_tmp
         mv $var_tmp $var
         
         t="s/\$constructive/$constructive/g"  
         sed $t < $var > $var_tmp
         mv $var_tmp $var
    else echo "8.$i Creating Constructive $constructive (CPP)...[fail]"
         exit
    fi


done


echo "9. ProblemCommand is deprecated, ignoring..."

# Closing project file

echo "#endif /*${project}_H_*/" >> "$workdir/$project/src/$project.h"

##############################################
#             Main file
##############################################

var="$workdir/$project/src/main$project.cpp"
var_tmp=$var".tmp"

if cp ./mct/main.tpl $var
then echo "Main file...[ok]"
     
     t="s/\$project/$project/g"  
     sed "$t" < $var > $var_tmp
     mv $var_tmp $var

     if [ $nbISG -eq 0 ];
     then t="/Constructive\$constructive c1(p);/,+1 d"  
	  sed "$t" < $var > $var_tmp
    	  mv $var_tmp $var
	  t="/check.add(c1);/d"
	  sed "$t" < $var > $var_tmp
     else t="s/\$constructive/$constructive/g"  
          sed "$t" < $var > $var_tmp
     fi
     mv $var_tmp $var

     if [ $nbNS -eq 0 ];
     then t="/NSSeq\$neighborhood ns1(p, rg);/,+1 d"
	  sed "$t" < $var > $var_tmp
    	  mv $var_tmp $var
	  t="/check.add(ns1);/d"
	  sed "$t" < $var > $var_tmp
     else t="s/\$neighborhood/$neighborhood/g"
	  sed "$t" < $var > $var_tmp	
     fi
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
var="$workdir/$project/makefile"
var_tmp="makefile.tmp"

if cp ./mct/makefile.tpl $var
then echo "makefile file...[ok]"

     if [ $nbISG -eq 0 ];
     then t="/Constructive\$constructive.cpp/,+2 d"  
	  sed "$t" < $var > $var_tmp
     	  mv $var_tmp $var
	  t="s/\ \$projectConstructive\$constructive.o\ //g"
          sed "$t" < $var > $var_tmp
     else t="s/\$constructive/$constructive/g"  
          sed "$t" < $var > $var_tmp
     fi
     mv $var_tmp $var
     
     if [ $nbNS -eq 0 ];
     then t="/NSSeq\$neighborhood.cpp/,+2 d"
	  sed "$t" < $var > $var_tmp
     	  mv $var_tmp $var
	  t="s/\ \$projectNSSeq\$neighborhood.o\ //g"
          sed "$t" < $var > $var_tmp
     else t="s/\$neighborhood/$neighborhood/g"
	  sed "$t" < $var > $var_tmp	
     fi
     mv $var_tmp $var
     
     t="s/\$project/$project/g"  
     sed "$t" < $var > $var_tmp
     mv $var_tmp $var

else echo "makefile...[fail]"
     exit
fi

if [ -f .$workdir/$project/makefile ]
then
    echo "Error! makefile already exists!"
    exit
else
    echo Creating basic makefile with the project.
    if cp ./mct/makefile.tpl $workdir/$project/makefile
    then echo "basic makefile file... $workdir/$project/makefile [ok]"
    else echo "basic makefile...[fail]"
      exit
    fi
    otherProj=NONE
    echo "cat $workdir/$project/makefile"
    cat $workdir/$project/makefile 
    #mv  $workdir/$project/makefile $workdir/$project/makefile.tmp
fi

#echo "VAR = $var "
#cat $var $workdir/$project/makefile.tmp > $workdir/$project/makefile

#rm -f $var
#rm -f $workdir/$project/makefile.tmp

echo
echo "Congratulations! You can use the following command to compile your project:"
if [ $nbNS -eq 0 ] && [ $nbISG -eq 0 ];
then echo "g++ --std=c++17 -fconcepts $workdir/main$project.cpp $workdir/$project/ProblemInstance.cpp $workdir/$project/Evaluator.cpp ../src/OptFrame/Scanner++/Scanner.cpp -o MyProjects/app_$project"
elif [ $nbNS -eq 0 ];
then echo "g++ --std=c++17 -fconcepts $workdir/main$project.cpp $workdir/$project/ProblemInstance.cpp $workdir/$project/Evaluator.cpp $workdir/$project/Constructive$constructive.cpp ../src/OptFrame/Scanner++/Scanner.cpp -o MyProjects/app_$project"
elif [ $nbISG -eq 0 ];
then echo "g++ --std=c++17 -fconcepts $workdir/main$project.cpp $workdir/$project/ProblemInstance.cpp $workdir/$project/Evaluator.cpp $workdir/$project/NSSeq$neighborhood.cpp ../src/OptFrame/Scanner++/Scanner.cpp -o MyProjects/app_$project"
else echo "g++ --std=c++17 -fconcepts $workdir/main$project.cpp $workdir/$project/ProblemInstance.cpp $workdir/$project/Evaluator.cpp $workdir/$project/Constructive$constructive.cpp $workdir/$project/NSSeq$neighborhood.cpp ../src/OptFrame/Scanner++/Scanner.cpp -o MyProjects/app_$project"
fi
echo
echo "or you can simply type: \"cd ../MyProjects/$project && make\""

echo
echo "Goodbye!"
