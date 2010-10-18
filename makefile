all:
	echo OptFrame default compilation.
	g++ main.cpp ./OptFrame/Util/Scanner++/Scanner.cpp -lpthread -o mainOptFrame
	g++ mainmp.cpp ./OptFrame/Util/Scanner++/Scanner.cpp -fopenmp -o mainOptFrameMP

vrptw:	
	g++ mainVRPTW.cpp OptFrame/Util/Scanner++/Scanner.cpp -o mainVRPTW -O3 -fopenmp -Dmulticore
