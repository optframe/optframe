all:
	g++ -Wall -DOPTFRAME_PARALLEL -lpthread main.cpp ./OptFrame/Scanner++/Scanner.cpp -o mainOptFrame

examples: tsp eternityII pmedcap

tsp:
	g++ -Wall ./Examples/mainTSP.cpp ./OptFrame/Scanner++/Scanner.cpp -o TSPExample

eternityII:
	g++ -Wall ./Examples/mainEternityII.cpp ./OptFrame/Scanner++/Scanner.cpp -o EternityIIExample

pmedcap:
	g++ -Wall ./Examples/mainPMedCap.cpp ./OptFrame/Scanner++/Scanner.cpp -o PMedCapExample
	
clean: 
	rm mainOptFrame
	rm TSPExample
	rm EternityIIExample
	rm PMedCapIIExample
