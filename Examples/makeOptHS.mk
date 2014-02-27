all: app_OptHS

CC=g++
CFLAGS=-O3 -Wall

app_OptHS:  OptHSmain.o  OptHSScanner.o  OptHSConstructiveRandom.o  OptHSEvaluator.o  OptHSNSSwap.o  OptHSProblemCommand.o  OptHSProblemInstance.o  
	$(CC) $(CFLAGS) OptHS*.o -o $@

OptHSScanner.o: ../OptFrame/Scanner++/Scanner.cpp  ../OptFrame/Scanner++/Scanner.h
	$(CC) -c $(CFLAGS) ../OptFrame/Scanner++/Scanner.cpp -o OptHSScanner.o

OptHSmain.o: mainOptHScheduling.cpp
	$(CC) -c $(CFLAGS) mainOptHScheduling.cpp -o $@
	
OptHSConstructiveRandom.o: OptHScheduling/ConstructiveRandom.cpp OptHScheduling/ConstructiveRandom.h
	$(CC) -c $(CFLAGS) OptHScheduling/ConstructiveRandom.cpp -o $@

OptHSEvaluator.o: OptHScheduling/Evaluator.cpp OptHScheduling/Evaluator.h
	$(CC) -c $(CFLAGS) OptHScheduling/Evaluator.cpp -o $@

OptHSNSSwap.o: OptHScheduling/NSSwap.cpp OptHScheduling/NSSwap.h
	$(CC) -c $(CFLAGS) OptHScheduling/NSSwap.cpp -o $@
	
OptHSProblemCommand.o: OptHScheduling/ProblemCommand.cpp OptHScheduling/ProblemCommand.h
	$(CC) -c $(CFLAGS) OptHScheduling/ProblemCommand.cpp -o $@

OptHSProblemInstance.o: OptHScheduling/ProblemInstance.cpp OptHScheduling/ProblemInstance.h
	$(CC) -c $(CFLAGS) OptHScheduling/ProblemInstance.cpp -o $@

	
.PHONY: all clean

clean: 
	rm -f app_OptHS
	rm -f OptHS*.o
	