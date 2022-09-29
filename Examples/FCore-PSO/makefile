all: fcore-pso

fcore-pso:
	mkdir -p build
	# g++ -I../../include -I../../libs/ --std=c++17 -fconcepts  -fconcepts-diagnostics-depth=2            mainPSO.cpp  -o ./build/app_PSO-fcore #2> error-gcc.err 
	g++ -I../../include -I../../libs/ --std=c++20   -fconcepts-diagnostics-depth=2            mainPSO.cpp  -o ./build/app_PSO-fcore #2> error-gcc.err 

run:
	@echo "Running FCore (c++17)"
	./build/app_PSO-fcore 
