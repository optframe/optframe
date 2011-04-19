all: examples
	g++ main.cpp ./OptFrame/Scanner++/Scanner.cpp -lpthread -o mainOptFrame

examples: tsp eternityII	

tsp:
	g++ ./Examples/mainTSP.cpp ./OptFrame/Scanner++/Scanner.cpp -lpthread -o mainOptFrameTSP

eternityII:
	g++ ./Examples/mainEternityII.cpp ./OptFrame/Scanner++/Scanner.cpp -lpthread -o mainOptFrameEternityII

	
clean: 
	rm mainOptFrame
	rm mainOptFrameTSP
	rm mainOptFrameEternityII