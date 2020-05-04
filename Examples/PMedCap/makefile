all: app_PMedCap

CC=g++
CFLAGS=-O3 -I../../src -Wall --std=c++17 -fconcepts -fno-exceptions -fno-rtti #TODO: disable exceptions

app_PMedCap:  PMedCapmain.o PMedCapInitialSolutionGreedy.o PMedCapInitialSolutionRandom.o  PMedCapInitialSolutionRandomGreedy.o  PMedCapNSSeqSwap.o  PMedCapEvaluator.o  PMedCapNSSeqSwap.o   PMedCapProblemInstance.o   
	$(CC) $(CFLAGS) PMedCap*.o -o $@

PMedCapmain.o: mainPMedCap.cpp
	$(CC) -c $(CFLAGS) mainPMedCap.cpp -o $@
	
PMedCapInitialSolutionGreedy.o: src/InitialSolutionGreedy.cpp src/InitialSolutionGreedy.h
	$(CC) -c $(CFLAGS) src/InitialSolutionGreedy.cpp -o $@

PMedCapInitialSolutionRandom.o: src/InitialSolutionRandom.cpp src/InitialSolutionRandom.h
	$(CC) -c $(CFLAGS) src/InitialSolutionRandom.cpp -o $@

PMedCapInitialSolutionRandomGreedy.o: src/InitialSolutionRandomGreedy.cpp src/InitialSolutionRandomGreedy.h
	$(CC) -c $(CFLAGS) src/InitialSolutionRandomGreedy.cpp -o $@

PMedCapNSSeqSwap.o: src/NSSeqSWAP.cpp src/NSSeqSWAP.h
	$(CC) -c $(CFLAGS) src/NSSeqSWAP.cpp -o $@

PMedCapEvaluator.o: src/Evaluator.cpp src/Evaluator.h
	$(CC) -c $(CFLAGS) src/Evaluator.cpp -o $@

PMedCapProblemInstance.o: src/ProblemInstance.cpp src/ProblemInstance.h
	$(CC) -c $(CFLAGS) src/ProblemInstance.cpp -o $@

	
.PHONY: all clean

clean: 
	rm -f app_PMedCap
	rm -f PMedCap*.o
	
