all: fcore-pso

fcore-pso:
	mkdir -p build
	g++ -I../../src -I../../libs/ ../../src/OptFrame/printable/printable.cpp --std=c++17 -fconcepts              mainPSO.cpp  -o ./build/app_PSO-fcore #2> error-gcc.err 

run:
	@echo "Running FCore (c++17)"
	./build/app_PSO-fcore 
