all: fcore-tsp #fxcore-tsp

fcore-tsp:
	g++    -I../../src -I../../libs/ ../../src/OptFrame/printable/printable.cpp  --std=c++17 -fconcepts              src/mainTSP-fcore.cpp  -o ./build/app_TSP-fcore #2> error-gcc.err 

fxcore-tsp:
	g++-11 -fsanitize=address -g -Wfatal-errors -I../../src -I../../libs/ --std=c++20 -fconcepts -fcoroutines src/mainTSP-fxcore.cpp -o ./build/app_TSP-fxcore #2> error-gcc.err 

run:
	@echo "Running FCore (c++17)"
	./build/app_TSP-fcore 
	@echo ""
	@echo "Running FxCore (c++20 experimental)"
	./build/app_TSP-fxcore 
