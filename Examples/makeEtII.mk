all: app_EtII

CC=g++
CFLAGS=-O3 -Wall

app_EtII:  EtIImain.o  EtIIScanner.o  EtIIInitialSolutionGreedy.o EtIIInitialSolutionRandom.o EtIIDeltaMoveRotate.o  EtIIDeltaMoveSwapCenter.o  EtIIDeltaMoveSwapCorner.o  EtIIDeltaMoveSwapRotateCenter.o  EtIIDeltaMoveSwapSide.o  EtIIEvaluator.o  EtIINSSeqRotate.o  EtIINSSeqSwapCenter.o  EtIINSSeqSwapCorner.o  EtIINSSeqSwapRotateCenter.o  EtIINSSeqSwapSide.o    EtIIProblemCommand.o  EtIIProblemInstance.o   
	$(CC) $(CFLAGS) EtII*.o -o $@

EtIIScanner.o: ../OptFrame/Scanner++/Scanner.cpp  ../OptFrame/Scanner++/Scanner.h
	$(CC) -c $(CFLAGS) ../OptFrame/Scanner++/Scanner.cpp -o EtIIScanner.o

EtIImain.o: mainEternityII.cpp
	$(CC) -c $(CFLAGS) mainEternityII.cpp -o $@
	
EtIIInitialSolutionGreedy.o: EternityII/InitialSolutionGreedy.cpp EternityII/InitialSolutionGreedy.h
	$(CC) -c $(CFLAGS) EternityII/InitialSolutionGreedy.cpp -o $@
	
EtIIInitialSolutionRandom.o: EternityII/InitialSolutionRandom.cpp EternityII/InitialSolutionRandom.h
	$(CC) -c $(CFLAGS) EternityII/InitialSolutionRandom.cpp -o $@

EtIIDeltaMoveRotate.o: EternityII/DeltaMoveRotate.cpp EternityII/DeltaMoveRotate.h
	$(CC) -c $(CFLAGS) EternityII/DeltaMoveRotate.cpp -o $@
	
EtIIDeltaMoveSwapCenter.o: EternityII/DeltaMoveSwapCenter.cpp EternityII/DeltaMoveSwapCenter.h
	$(CC) -c $(CFLAGS) EternityII/DeltaMoveSwapCenter.cpp -o $@

EtIIDeltaMoveSwapCorner.o: EternityII/DeltaMoveSwapCorner.cpp EternityII/DeltaMoveSwapCorner.h
	$(CC) -c $(CFLAGS) EternityII/DeltaMoveSwapCorner.cpp -o $@

EtIIDeltaMoveSwapRotateCenter.o: EternityII/DeltaMoveSwapRotateCenter.cpp EternityII/DeltaMoveSwapRotateCenter.h
	$(CC) -c $(CFLAGS) EternityII/DeltaMoveSwapRotateCenter.cpp -o $@

EtIIDeltaMoveSwapSide.o: EternityII/DeltaMoveSwapSide.cpp EternityII/DeltaMoveSwapSide.h
	$(CC) -c $(CFLAGS) EternityII/DeltaMoveSwapSide.cpp -o $@

EtIIEvaluator.o: EternityII/Evaluator.cpp EternityII/Evaluator.h
	$(CC) -c $(CFLAGS) EternityII/Evaluator.cpp -o $@

EtIINSSeqRotate.o: EternityII/NSSeqRotate.cpp EternityII/NSSeqRotate.h
	$(CC) -c $(CFLAGS) EternityII/NSSeqRotate.cpp -o $@
	
EtIINSSeqSwapCenter.o: EternityII/NSSeqSwapCenter.cpp EternityII/NSSeqSwapCenter.h
	$(CC) -c $(CFLAGS) EternityII/NSSeqSwapCenter.cpp -o $@
	
EtIINSSeqSwapCorner.o: EternityII/NSSeqSwapCorner.cpp EternityII/NSSeqSwapCorner.h
	$(CC) -c $(CFLAGS) EternityII/NSSeqSwapCorner.cpp -o $@
	
EtIINSSeqSwapRotateCenter.o: EternityII/NSSeqSwapRotateCenter.cpp EternityII/NSSeqSwapRotateCenter.h
	$(CC) -c $(CFLAGS) EternityII/NSSeqSwapRotateCenter.cpp -o $@
	
EtIINSSeqSwapSide.o: EternityII/NSSeqSwapSide.cpp EternityII/NSSeqSwapSide.h
	$(CC) -c $(CFLAGS) EternityII/NSSeqSwapSide.cpp -o $@
	
EtIIProblemCommand.o: EternityII/ProblemCommand.cpp EternityII/ProblemCommand.h
	$(CC) -c $(CFLAGS) EternityII/ProblemCommand.cpp -o $@

EtIIProblemInstance.o: EternityII/ProblemInstance.cpp EternityII/ProblemInstance.h
	$(CC) -c $(CFLAGS) EternityII/ProblemInstance.cpp -o $@

	
.PHONY: all clean

clean: 
	rm -f app_EtII
	rm -f EtII*.o
	