all:
	g++ -DALL_INTEGER -O3 mainSVRPDSP.cpp ../OptFrame/Scanner++/Scanner.cpp -o app_SVRPDSP