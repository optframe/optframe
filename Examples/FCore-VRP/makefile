all: mainTSP-brkga

mainTSP-brkga:
	mkdir -p build
	g++ -I../../include -I../../libs/ -Wfatal-errors --std=c++17 -fconcepts mainVRP.cpp -o ./build/mainTSP-brkga #2> error-gcc.err 

run:
	@echo "Running FCore (c++17)"
	./build/mainTSP-brkga 
