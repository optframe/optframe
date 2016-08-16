all:
	g++ --std=c++11 mainHFMVRP.cpp ../OptFrame/Scanner++/Scanner.cpp -O3 -o app_HFMVRP
