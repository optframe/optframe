all: examples
	g++ -Wall main.cpp ./OptFrame/Scanner++/Scanner.cpp -lpthread -o mainOptFrame

examples: tsp eternityII	

tsp:
	g++ -Wall ./Examples/mainTSP.cpp ./OptFrame/Scanner++/Scanner.cpp -lpthread -o mainOptFrameTSP

eternityII:
	g++ -Wall ./Examples/mainEternityII.cpp ./OptFrame/Scanner++/Scanner.cpp -lpthread -o mainOptFrameEternityII

	
clean: 
	rm mainOptFrame
	rm mainOptFrameTSP
	rm mainOptFrameEternityII