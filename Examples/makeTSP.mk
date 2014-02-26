all: app_TSP

CC=g++
CFLAGS=-O3 -Wall

app_TSP:  TSPmain.o  TSPScanner.o  TSPConstructiveBestInsertion.o  TSPDeltaMoveTSP2Opt.o  TSPDeltaMoveTSPOrOptk.o  TSPEvaluator.o  TSPNearestNeighborConstructive.o  TSPNSEnumShift.o  TSPNSEnumSwap.o  TSPProblemCommand.o  TSPProblemInstance.o  TSPRandomInitialSolution.o  
	$(CC) $(CFLAGS) TSP*.o -o $@

TSPScanner.o: ../OptFrame/Scanner++/Scanner.cpp  ../OptFrame/Scanner++/Scanner.h
	$(CC) -c $(CFLAGS) ../OptFrame/Scanner++/Scanner.cpp -o TSPScanner.o

TSPmain.o: mainTSP.cpp
	$(CC) -c $(CFLAGS) mainTSP.cpp -o $@
	
TSPConstructiveBestInsertion.o: TSP/ConstructiveBestInsertion.cpp TSP/ConstructiveBestInsertion.h
	$(CC) -c $(CFLAGS) TSP/ConstructiveBestInsertion.cpp -o $@

TSPDeltaMoveTSP2Opt.o: TSP/DeltaMoveTSP2Opt.cpp TSP/DeltaMoveTSP2Opt.h
	$(CC) -c $(CFLAGS) TSP/DeltaMoveTSP2Opt.cpp -o $@

TSPDeltaMoveTSPOrOptk.o: TSP/DeltaMoveTSPOrOptk.cpp TSP/DeltaMoveTSPOrOptk.h
	$(CC) -c $(CFLAGS) TSP/DeltaMoveTSPOrOptk.cpp -o $@

TSPEvaluator.o: TSP/Evaluator.cpp TSP/Evaluator.h
	$(CC) -c $(CFLAGS) TSP/Evaluator.cpp -o $@

TSPNearestNeighborConstructive.o: TSP/NearestNeighborConstructive.cpp TSP/NearestNeighborConstructive.h
	$(CC) -c $(CFLAGS) TSP/NearestNeighborConstructive.cpp -o $@
	
TSPNSEnumShift.o: TSP/NSEnumShift.cpp TSP/NSEnumShift.h
	$(CC) -c $(CFLAGS) TSP/NSEnumShift.cpp -o $@
	
TSPNSEnumSwap.o: TSP/NSEnumSwap.cpp TSP/NSEnumSwap.h
	$(CC) -c $(CFLAGS) TSP/NSEnumSwap.cpp -o $@

TSPProblemCommand.o: TSP/ProblemCommand.cpp TSP/ProblemCommand.h
	$(CC) -c $(CFLAGS) TSP/ProblemCommand.cpp -o $@

TSPProblemInstance.o: TSP/ProblemInstance.cpp TSP/ProblemInstance.h
	$(CC) -c $(CFLAGS) TSP/ProblemInstance.cpp -o $@

TSPRandomInitialSolution.o: TSP/RandomInitialSolution.cpp TSP/RandomInitialSolution.h
	$(CC) -c $(CFLAGS) TSP/RandomInitialSolution.cpp -o $@
	
	
.PHONY: all clean

clean: 
	rm -f app_TSP
	rm -f TSP*.o
	