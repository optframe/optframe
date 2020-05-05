all: app_OptHS

CC=g++
CFLAGS= -I../../src  -fno-exceptions -O3 -Wall --std=c++17 -fconcepts #-fno-exceptions -fno-rtti #TODO: disable exceptions

app_OptHS:  OptHSmain.o   OptHSConstructiveRandom.o  OptHSEvaluator.o  OptHSNSSwap.o  OptHSProblemCommand.o  OptHSProblemInstance.o  
	$(CC) $(CFLAGS) OptHS*.o -o $@

OptHSmain.o: mainOptHScheduling.cpp
	$(CC) -c $(CFLAGS) mainOptHScheduling.cpp -o $@
	
OptHSConstructiveRandom.o: src/ConstructiveRandom.cpp src/ConstructiveRandom.h
	$(CC) -c $(CFLAGS) src/ConstructiveRandom.cpp -o $@

OptHSEvaluator.o: src/Evaluator.cpp src/Evaluator.h
	$(CC) -c $(CFLAGS) src/Evaluator.cpp -o $@

OptHSNSSwap.o: src/NSSwap.cpp src/NSSwap.h
	$(CC) -c $(CFLAGS) src/NSSwap.cpp -o $@
	
OptHSProblemCommand.o: src/ProblemCommand.cpp src/ProblemCommand.h
	$(CC) -c $(CFLAGS) src/ProblemCommand.cpp -o $@

OptHSProblemInstance.o: src/ProblemInstance.cpp src/ProblemInstance.h
	$(CC) -c $(CFLAGS) src/ProblemInstance.cpp -o $@

	
.PHONY: all clean

clean: 
	rm -f app_OptHS
	rm -f OptHS*.o
	