all: kp01_generic

kp01_generic:
	g++ -Isrc -Ilibs -I../../include --std=c++20 src/solver_kp01_generic/mainSolverKP.cpp src/solver_kp01_generic/solver_kp01_generic.cpp  -o ./build/app_KP01_generic

run:
	@echo "Running FCore (c++20)"
	./build/app_KP01_generic
