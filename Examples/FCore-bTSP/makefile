all: fcore-btsp 

#  -Wfatal-errors 
fcore-btsp:
	g++  -I../../include -I../../libs/  -fconcepts-diagnostics-depth=3 --std=c++20   src/mainBTSP-fcore.cpp  -o ./build/app_bTSP-fcore

run:
	@echo "Running FCore (c++20)"
	./build/app_bTSP-fcore 

clean:
	rm -f ./build/app_*
