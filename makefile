all:
	echo OptFrame Beta compilation.
	g++ main.cpp ./OptFrame/Util/Scanner++/Scanner.cpp -lpthread -o mainOptFrame
