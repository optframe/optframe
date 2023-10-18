all: demo_quickstart_welcome 

demo_quickstart_welcome:	
	g++ --std=c++20 -I../../include mytest.cpp -o demo_welcome
	# Expects: Welcome to OptFrame Functional Core (FCore) - version 4.1-dev
	./demo_welcome
	

build_with_bazel:
	bazel build ... --cxxopt=-std=c++17

