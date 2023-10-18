all: mainTSP-brkga

mainTSP-brkga:
	mkdir -p build
	g++ -I../../include -I../../libs/ ../../src/OptFrame/printable/printable.cpp --std=c++20 mainTSP-brkga.cpp -o ./build/mainTSP-brkga #2> error-gcc.err 

run:
	@echo "Running FCore (c++17)"
	./build/mainTSP-brkga 