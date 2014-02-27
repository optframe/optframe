all: app_PMedCap

CC=g++
CFLAGS=-O3 -Wall

app_PMedCap:  PMedCapmain.o  PMedCapScanner.o  PMedCapInitialSolutionGreedy.o PMedCapInitialSolutionRandom.o  PMedCapInitialSolutionRandomGreedy.o  PMedCapNSSeqSwap.o  PMedCapEvaluator.o  PMedCapNSSeqSwap.o   PMedCapProblemInstance.o   
	$(CC) $(CFLAGS) PMedCap*.o -o $@

PMedCapScanner.o: ../OptFrame/Scanner++/Scanner.cpp  ../OptFrame/Scanner++/Scanner.h
	$(CC) -c $(CFLAGS) ../OptFrame/Scanner++/Scanner.cpp -o PMedCapScanner.o

PMedCapmain.o: mainPMedCap.cpp
	$(CC) -c $(CFLAGS) mainPMedCap.cpp -o $@
	
PMedCapInitialSolutionGreedy.o: PMedCap/InitialSolutionGreedy.cpp PMedCap/InitialSolutionGreedy.h
	$(CC) -c $(CFLAGS) PMedCap/InitialSolutionGreedy.cpp -o $@

PMedCapInitialSolutionRandom.o: PMedCap/InitialSolutionRandom.cpp PMedCap/InitialSolutionRandom.h
	$(CC) -c $(CFLAGS) PMedCap/InitialSolutionRandom.cpp -o $@

PMedCapInitialSolutionRandomGreedy.o: PMedCap/InitialSolutionRandomGreedy.cpp PMedCap/InitialSolutionRandomGreedy.h
	$(CC) -c $(CFLAGS) PMedCap/InitialSolutionRandomGreedy.cpp -o $@

PMedCapNSSeqSwap.o: PMedCap/NSSeqSWAP.cpp PMedCap/NSSeqSWAP.h
	$(CC) -c $(CFLAGS) PMedCap/NSSeqSWAP.cpp -o $@

PMedCapEvaluator.o: PMedCap/Evaluator.cpp PMedCap/Evaluator.h
	$(CC) -c $(CFLAGS) PMedCap/Evaluator.cpp -o $@

PMedCapProblemInstance.o: PMedCap/ProblemInstance.cpp PMedCap/ProblemInstance.h
	$(CC) -c $(CFLAGS) PMedCap/ProblemInstance.cpp -o $@

	
.PHONY: all clean

clean: 
	rm -f app_PMedCap
	rm -f PMedCap*.o
	