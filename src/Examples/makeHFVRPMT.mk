all: app_HFMVRP

app_HFMVRP:
	g++ --std=c++17 -fconcepts -g mainHFMVRP.cpp ../OptFrame/Scanner++/Scanner.cpp -Ofast -o app_HFMVRP
