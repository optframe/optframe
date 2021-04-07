all: mainTSP-brkga

mainTSP-brkga:
	mkdir -p build
	g++ -I../../src -I../../libs/ ../../src/OptFrame/printable/printable.cpp --std=c++17 -fconcepts mainTSP-brkga.cpp -o ./build/mainTSP-brkga #2> error-gcc.err 

run:
	@echo "Running FCore (c++17)"
	./build/mainTSP-brkga 