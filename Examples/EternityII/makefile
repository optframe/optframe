all: app_EtII

CC=g++
CFLAGS= -I../../src  -fno-exceptions -O3 -Wall --std=c++17 -fconcepts #-fno-exceptions -fno-rtti #TODO: disable exceptions

app_EtII:  EtIImain.o   EtIIInitialSolutionGreedy.o EtIIInitialSolutionRandom.o EtIIDeltaMoveRotate.o  EtIIDeltaMoveSwapCenter.o  EtIIDeltaMoveSwapCorner.o  EtIIDeltaMoveSwapRotateCenter.o  EtIIDeltaMoveSwapSide.o  EtIIEvaluator.o  EtIINSSeqRotate.o  EtIINSSeqSwapCenter.o  EtIINSSeqSwapCorner.o  EtIINSSeqSwapRotateCenter.o  EtIINSSeqSwapSide.o    EtIIProblemCommand.o  EtIIProblemInstance.o   
	$(CC) $(CFLAGS) EtII*.o -o $@

EtIImain.o: mainEternityII.cpp
	$(CC) -c $(CFLAGS) mainEternityII.cpp -o $@
	
EtIIInitialSolutionGreedy.o: src/InitialSolutionGreedy.cpp src/InitialSolutionGreedy.h
	$(CC) -c $(CFLAGS) src/InitialSolutionGreedy.cpp -o $@
	
EtIIInitialSolutionRandom.o: src/InitialSolutionRandom.cpp src/InitialSolutionRandom.h
	$(CC) -c $(CFLAGS) src/InitialSolutionRandom.cpp -o $@

EtIIDeltaMoveRotate.o: src/DeltaMoveRotate.cpp src/DeltaMoveRotate.h
	$(CC) -c $(CFLAGS) src/DeltaMoveRotate.cpp -o $@
	
EtIIDeltaMoveSwapCenter.o: src/DeltaMoveSwapCenter.cpp src/DeltaMoveSwapCenter.h
	$(CC) -c $(CFLAGS) src/DeltaMoveSwapCenter.cpp -o $@

EtIIDeltaMoveSwapCorner.o: src/DeltaMoveSwapCorner.cpp src/DeltaMoveSwapCorner.h
	$(CC) -c $(CFLAGS) src/DeltaMoveSwapCorner.cpp -o $@

EtIIDeltaMoveSwapRotateCenter.o: src/DeltaMoveSwapRotateCenter.cpp src/DeltaMoveSwapRotateCenter.h
	$(CC) -c $(CFLAGS) src/DeltaMoveSwapRotateCenter.cpp -o $@

EtIIDeltaMoveSwapSide.o: src/DeltaMoveSwapSide.cpp src/DeltaMoveSwapSide.h
	$(CC) -c $(CFLAGS) src/DeltaMoveSwapSide.cpp -o $@

EtIIEvaluator.o: src/Evaluator.cpp src/Evaluator.h
	$(CC) -c $(CFLAGS) src/Evaluator.cpp -o $@

EtIINSSeqRotate.o: src/NSSeqRotate.cpp src/NSSeqRotate.h
	$(CC) -c $(CFLAGS) src/NSSeqRotate.cpp -o $@
	
EtIINSSeqSwapCenter.o: src/NSSeqSwapCenter.cpp src/NSSeqSwapCenter.h
	$(CC) -c $(CFLAGS) src/NSSeqSwapCenter.cpp -o $@
	
EtIINSSeqSwapCorner.o: src/NSSeqSwapCorner.cpp src/NSSeqSwapCorner.h
	$(CC) -c $(CFLAGS) src/NSSeqSwapCorner.cpp -o $@
	
EtIINSSeqSwapRotateCenter.o: src/NSSeqSwapRotateCenter.cpp src/NSSeqSwapRotateCenter.h
	$(CC) -c $(CFLAGS) src/NSSeqSwapRotateCenter.cpp -o $@
	
EtIINSSeqSwapSide.o: src/NSSeqSwapSide.cpp src/NSSeqSwapSide.h
	$(CC) -c $(CFLAGS) src/NSSeqSwapSide.cpp -o $@
	
EtIIProblemCommand.o: src/ProblemCommand.cpp src/ProblemCommand.h
	$(CC) -c $(CFLAGS) src/ProblemCommand.cpp -o $@

EtIIProblemInstance.o: src/ProblemInstance.cpp src/ProblemInstance.h
	$(CC) -c $(CFLAGS) src/ProblemInstance.cpp -o $@

	
.PHONY: all clean

clean: 
	rm -f app_EtII
	rm -f EtII*.o
	