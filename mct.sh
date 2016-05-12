#!/bin/bash
#######################################################################
# OptFrame - Project Generator MCT - "Make a Compilable Thing!"
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
echo "1. What's the name of your project? (Step 1 of 9)"
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

if mkdir ./MyProjects/$project
then
  echo "Creating project directory...[ok]"
else
  echo "Creating project directory...[fail]"
  exit
fi

##############################################
#  Creating files
##############################################

if echo > "./MyProjects/$project.h"
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
echo "$var" > "./MyProjects/$project.h"
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


var_inc="./$project/Representation.h"
var="./MyProjects/$project/Representation.h"
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

     echo "#include \"$var_inc\"" >> ./MyProjects/$project.h
else echo "1. Creating Representation File...[fail]"
     exit
fi


## Creating Solution file

var_inc="./$project/Solution.h"
var="./MyProjects/$project/Solution.h"
var_tmp=$var".tmp"

if cp ./mct/Solution.tpl $var
then echo "2. Creating Solution File...[ok]"

     t="s/\$project/$project/g"  
     sed $t < $var > $var_tmp
     mv $var_tmp $var

     echo "#include \"$var_inc\"" >> ./MyProjects/$project.h
else echo "2. Creating Solution File...[fail]"
     exit
fi


##############################################
#          Delta Structure
##############################################
echo "3. Delta Structure is deprecated, use the Solution ADS instead. Moving to next question..."

## Creating Evaluation file

var_inc="./$project/Evaluation.h"
var="./MyProjects/$project/Evaluation.h"
var_tmp=$var".tmp"

if cp ./mct/Evaluation.tpl $var
then echo "4. Creating Evaluation File...[ok]"

     t="s/\$project/$project/g"  
     sed $t < $var > $var_tmp
     mv $var_tmp $var

     echo "#include \"$var_inc\"" >> ./MyProjects/$project.h
else echo "4. Creating Evaluation File...[fail]"
     exit
fi


##############################################
#             Problem Instance
##############################################

var_inc="./$project/ProblemInstance.h"
var="./MyProjects/$project/ProblemInstance.h"
var_tmp=$var".tmp"

if cp ./mct/ProblemInstance.tpl $var
then echo "5. Creating Problem Instance...[ok]"
     t="s/\$project/$project/g"  
     sed $t < $var > $var_tmp
     mv $var_tmp $var
     echo "#include \"$var_inc\"" >> ./MyProjects/$project.h
else echo "5. Creating Problem Instance...[fail]"
     exit
fi

var_inc="./$project/ProblemInstance.cpp"
var="./MyProjects/$project/ProblemInstance.cpp"
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

var_inc="./$project/Evaluator.h"
var="./MyProjects/$project/Evaluator.h"
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

     echo "#include \"$var_inc\"" >> ./MyProjects/$project.h
else echo "6. Creating Evaluator...[fail]"
     exit
fi

var_inc="./$project/Evaluator.cpp"
var="./MyProjects/$project/Evaluator.cpp"
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

    var_inc="./$project/NSSeq$neighborhood.h"
    var="./MyProjects/$project/NSSeq$neighborhood.h"
    var_tmp=$var".tmp"

    if cp ./mct/NSSeq.tpl $var
    then echo "7.$i Creating Neighborhood Structure $neighborhood ...[ok]"
         t="s/\$project/$project/g"  
         sed -e "$t" < $var > $var_tmp
         mv $var_tmp $var
         
         t="s/\$neighborhood/$neighborhood/g"  
         sed -e "$t" < $var > $var_tmp
         mv $var_tmp $var
         
         echo "#include \"$var_inc\"" >> ./MyProjects/$project.h
    else echo "7.$i Creating Neighborhood Structure $neighborhood ...[fail]"
         exit
    fi
    
    var_inc="./$project/NSSeq$neighborhood.cpp"
    var="./MyProjects/$project/NSSeq$neighborhood.cpp"
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

    var_inc="./$project/Constructive${constructive}.h"
    var="./MyProjects/$project/Constructive${constructive}.h"
    var_tmp=$var".tmp"

    if cp ./mct/Constructive.tpl $var
    then echo "8.$i Creating Constructive $constructive ...[ok]"
         t="s/\$project/$project/g"  
         sed $t < $var > $var_tmp
         mv $var_tmp $var
         
         t="s/\$constructive/$constructive/g"  
         sed $t < $var > $var_tmp
         mv $var_tmp $var
         
         echo "#include \"$var_inc\"" >> ./MyProjects/$project.h
    else echo "8.$i Creating Constructive $constructive ...[fail]"
         exit
    fi

    var_inc="./$project/Constructive${constructive}.cpp"
    var="./MyProjects/$project/Constructive${constructive}.cpp"
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

echo "#endif /*${project}_H_*/" >> "./MyProjects/$project.h"

##############################################
#             Main file
##############################################

var="./MyProjects/main$project.cpp"
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
var="./MyProjects/makefile_prj"
var_tmp="makefile.tmp"

if cp ./mct/makefile.tpl $var
then echo "makefile file...[ok]"
     
     t="s/\$project/$project/g"  
     sed "$t" < $var > $var_tmp
     mv $var_tmp $var
     
     t="s/\$constructive/$constructive/g"  
     sed "$t" < $var > $var_tmp
     mv $var_tmp $var
     
     t="s/\$neighborhood/$neighborhood/g"  
     sed "$t" < $var > $var_tmp
     mv $var_tmp $var

else echo "makefile...[fail]"
     exit
fi

if [ -f ./MyProjects/makefile ]
then
    echo Makefile already exists, just adding the project.
    # removing the option all from the old makefile
    sed '1,3d' ./MyProjects/makefile > ./MyProjects/makefile.tmp    
    #getting the name of the other projects
    otherProj=`cat ./MyProjects/makefile | sed -n 3p | sed 's/.*://'`
else
    echo Creating basic makefile with the project.
    if cp ./mct/makefile_base.tpl ./MyProjects/makefile
    then echo "basic makefile file...[ok]"
    else echo "basic makefile...[fail]"
      exit
    fi
    otherProj=NONE
    mv  ./MyProjects/makefile ./MyProjects/makefile.tmp
fi

cat $var ./MyProjects/makefile.tmp > ./MyProjects/makefile

if [ "$otherProj" != "NONE" ]
then
    sed -i "s/all: mctApp$project/all: $otherProj mctApp$project/" ./MyProjects/makefile
fi

rm -f $var
rm -f ./MyProjects/makefile.tmp

echo
echo "Congratulations! You can use the following command to compile your project:"
echo "g++ ./MyProjects/main$project.cpp ./MyProjects/$project/ProblemInstance.cpp ./MyProjects/$project/Evaluator.cpp ./MyProjects/$project/Constructive$constructive.cpp ./MyProjects/$project/NSSeq$neighborhood.cpp ./OptFrame/Scanner++/Scanner.cpp -o MyProjects/app_$project"
echo "or you can simply type: \"cd MyProjects && make\""

echo
echo "Goodbye!"
