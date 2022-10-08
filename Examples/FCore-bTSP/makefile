all: fcore-btsp 

#  -Wfatal-errors 
fcore-btsp:
#	g++  -I../../include -I../../libs/ -Wfatal-errors  -fconcepts-diagnostics-depth=3 --std=c++20   src/mainBTSP-fcore.cpp  -o ./build/app_bTSP-fcore
	g++  -I../../include -I../../libs/                 -fconcepts-diagnostics-depth=3 --std=c++20   src/mainBTSP-fcore.cpp  -o ./build/app_bTSP-fcore
#	clang-12  -I../../include -I../../libs/  -Wno-inconsistent-missing-override         --std=c++20   src/mainBTSP-fcore.cpp  -o ./build/app_bTSP-fcore

run:
	@echo "Running FCore (c++20)"
	./build/app_bTSP-fcore 

clean:
	rm -f ./build/app_*
