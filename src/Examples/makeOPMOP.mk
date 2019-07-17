all:
	g++ mainOPM.cpp -O3  --std=c++17 -fconcepts ../OptFrame/Scanner++/Scanner.cpp  -o  app_OPMOP
