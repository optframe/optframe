all: examples
	g++ -Wall main.cpp ./OptFrame/Scanner++/Scanner.cpp -o mainOptFrame

examples: tsp eternityII	

tsp:
	g++ -Wall ./Examples/mainTSP.cpp ./OptFrame/Scanner++/Scanner.cpp -o mainOptFrameTSP

eternityII:
	g++ -Wall ./Examples/mainEternityII.cpp ./OptFrame/Scanner++/Scanner.cpp -o mainOptFrameEternityII

	
clean: 
	rm mainOptFrame
	rm mainOptFrameTSP
	rm mainOptFrameEternityII