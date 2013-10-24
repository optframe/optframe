
GCC_FLAGS=-O3

examples: tsp eternityii pmedcap

tsp:
	g++ $(GCC_FLAGS) ../Examples/mainTSP.cpp ../OptFrame/Scanner++/Scanner.cpp -o ../Examples/TSPExample

eternityii:
	g++ $(GCC_FLAGS) ../Examples/mainEternityII.cpp ../OptFrame/Scanner++/Scanner.cpp -o ../Examples/EternityIIExample

pmedcap:
	g++ $(GCC_FLAGS) ../Examples/mainPMedCap.cpp ../OptFrame/Scanner++/Scanner.cpp -o ../Examples/PMedCapExample
	
clean: 
	rm -f ../Examples/TSPExample
	rm -f ../Examples/EternityIIExample
	rm -f ../Examples/PMedCapIIExample
	rm -f app_*
	rm -f *.o
