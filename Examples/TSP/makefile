all: app_TSP

#CC=g++-8
CC=g++
CFLAGS=  -I../../src  -fno-exceptions -g -Wall -O3 --std=c++17 -fconcepts #-fno-exceptions -fno-rtti #TODO: disable exceptions

app_TSP:  TSPmain.o  TSPConstructiveBestInsertion.o  TSPDeltaMoveTSP2Opt.o  TSPDeltaMoveTSPOrOptk.o  TSPEvaluator.o  TSPNearestNeighborConstructive.o  TSPNSEnumShift.o  TSPNSEnumSwap.o  TSPProblemCommand.o  TSPProblemInstance.o  TSPRandomInitialSolution.o  
	$(CC) $(CFLAGS) TSP*.o -o $@

TSPmain.o: mainTSP.cpp
	$(CC) -c $(CFLAGS) mainTSP.cpp -o $@
	
TSPConstructiveBestInsertion.o: src/ConstructiveBestInsertion.cpp src/ConstructiveBestInsertion.h
	$(CC) -c $(CFLAGS) src/ConstructiveBestInsertion.cpp -o $@

TSPDeltaMoveTSP2Opt.o: src/DeltaMoveTSP2Opt.cpp src/DeltaMoveTSP2Opt.h
	$(CC) -c $(CFLAGS) src/DeltaMoveTSP2Opt.cpp -o $@

TSPDeltaMoveTSPOrOptk.o: src/DeltaMoveTSPOrOptk.cpp src/DeltaMoveTSPOrOptk.h
	$(CC) -c $(CFLAGS) src/DeltaMoveTSPOrOptk.cpp -o $@

TSPEvaluator.o: src/Evaluator.cpp src/Evaluator.h
	$(CC) -c $(CFLAGS) src/Evaluator.cpp -o $@

TSPNearestNeighborConstructive.o: src/NearestNeighborConstructive.cpp src/NearestNeighborConstructive.h
	$(CC) -c $(CFLAGS) src/NearestNeighborConstructive.cpp -o $@
	
TSPNSEnumShift.o: src/NSEnumShift.cpp src/NSEnumShift.h
	$(CC) -c $(CFLAGS) src/NSEnumShift.cpp -o $@
	
TSPNSEnumSwap.o: src/NSEnumSwap.cpp src/NSEnumSwap.h
	$(CC) -c $(CFLAGS) src/NSEnumSwap.cpp -o $@

TSPProblemCommand.o: src/ProblemCommand.cpp src/ProblemCommand.h
	$(CC) -c $(CFLAGS) src/ProblemCommand.cpp -o $@

TSPProblemInstance.o: src/ProblemInstance.cpp src/ProblemInstance.h
	$(CC) -c $(CFLAGS) src/ProblemInstance.cpp -o $@

TSPRandomInitialSolution.o: src/RandomInitialSolution.cpp src/RandomInitialSolution.h
	$(CC) -c $(CFLAGS) src/RandomInitialSolution.cpp -o $@
	
	
.PHONY: all clean

clean: 
	rm -f app_TSP
	rm -f TSP*.o
	
