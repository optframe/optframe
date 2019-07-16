all: app_HFMVRP

app_HFMVRP:
	g++ --std=c++11 -g mainHFMVRP.cpp ../OptFrame/Scanner++/Scanner.cpp -Ofast -o app_HFMVRP
