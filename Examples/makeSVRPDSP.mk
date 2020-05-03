all:
	g++ --std=c++17 -fconcepts -DALL_INTEGER -O3 mainSVRPDSP.cpp ../OptFrame/Scanner++/Scanner.cpp -o app_SVRPDSP