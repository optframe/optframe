all: examples
	g++ -Wall main.cpp ./OptFrame/Scanner++/Scanner.cpp -o mainOptFrame

examples: tsp eternityII pmedcap

tsp:
	g++ -Wall ./Examples/mainTSP.cpp ./OptFrame/Scanner++/Scanner.cpp -o Examples/TSPExample

eternityII:
	g++ -Wall ./Examples/mainEternityII.cpp ./OptFrame/Scanner++/Scanner.cpp -o Examples/EternityIIExample

pmedcap:
	g++ -Wall ./Examples/mainPMedCap.cpp ./OptFrame/Scanner++/Scanner.cpp -o Examples/PMedCapExample
	
clean: 
	rm mainOptFrame
	rm Examples/TSPExample
	rm Examples/EternityIIExample
	rm Examples/PMedCapIIExample
