#!/bin/bash

# parameters: seq SeqMR MapMP MaPI

echo "Configuring parallel environment.."
export MALLOC_CHECK_=0

if [ "$1" = "seq" ] 
then
	echo "Running sequential application.."
	./$2 example.opt
	echo "End of sequential application.."
elif [ "$1" = "SeqMR" ] 
then
	echo "Running SeqMR application.."
	./$2 exampleSeqMR.opt
	echo "End of SeqMR application.."
elif [ "$1" = "MapMP" ] 
then
	echo "OptFrame+MapMP not implemented"
	#echo "Running MapMP application.."
	#./$2 exampleMapMP.opt
	#echo "End of MapMP application.."
elif [ "$1" = "MaPI" ] 
then
	echo "Running MaPI application.."
	NP=$3
	mpirun -np $NP $2 exampleMaPI.opt $NP
	echo "End of MaPI application.."
fi


echo "Finish.."
