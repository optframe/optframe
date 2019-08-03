GCC_FLAG_RELEASE 	= -std=c++11 -O3
GCC_FLAG_PRINT   	= -std=c++11 -O2 -DPRINT
GCC_FLAG_DEBUG  	= -std=c++11 -O1 -DDEBUG
GCC_FLAG_PRINT_DEBUG 	= -std=c++11 -O1 -DDEBUG -DPRINT
GCC_FLAG         	= ${GCC_FLAG_PRINT}

all: mainKP2.o Solution.o ProblemInstance.o Evaluator.o GeneticAlgorithm.o ProblemInstance.o
	g++ ${GCC_FLAG} mainKP2.o Solution.o Evaluator.o GeneticAlgorithm.o ProblemInstance.o -o app_KP2

mainKP2.o: KP2/Solution.h KP2/ProblemInstance.h KP2/Evaluator.h KP2/GeneticAlgorithm.h KP2.h mainKP2.cpp
	g++ ${GCC_FLAG} -c mainKP2.cpp -o mainKP2.o

Solution.o: KP2/Solution.h KP2/Solution.cpp
	g++ ${GCC_FLAG} -c KP2/Solution.cpp -o Solution.o

ProblemInstance.o: KP2/ProblemInstance.h KP2/ProblemInstance.cpp
	g++ ${GCC_FLAG} -c KP2/ProblemInstance.cpp -o ProblemInstance.o

Evaluator.o: KP2/Solution.h KP2/ProblemInstance.h KP2/Evaluator.h KP2/Evaluator.cpp
	g++ ${GCC_FLAG} -c KP2/Evaluator.cpp -o Evaluator.o

GeneticAlgorithm.o: KP2/Solution.h KP2/ProblemInstance.h KP2/Evaluator.h KP2/GeneticAlgorithm.h KP2/GeneticAlgorithm.cpp 
	g++ ${GCC_FLAG} -c KP2/GeneticAlgorithm.cpp -o GeneticAlgorithm.o

clean:
	rm -f *.o
	rm -f *.out

run:
	./app_KP2 < kp-simple > kp-simple.out
