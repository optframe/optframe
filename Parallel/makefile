all:	seq	MaPI
	
seq:	
	echo OptFrame Beta sequential compilation.
	g++ main.cpp ../OptFrame/Util/Scanner++/Scanner.cpp -lpthread -o mainOptFrame -O3

MaPI:
	echo OptFrame Beta MaPI compilation.
	mpiCC mainMaPI.cpp ../OptFrame/Util/Scanner++/Scanner.cpp -lpthread -o mainMaPI -O3
