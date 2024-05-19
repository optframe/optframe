all: fcore-brkga fcore-ils 

fcore-ils:
	echo "WILL JOIN FILES INTO COMPLETE EXAMPLE"
	./join.sh
	echo "WILL COMPILE"
	g++ -g -O3 --std=c++20 -I../../include   mainTSP-fcore-ils.cpp -o app_TSP-fcore-ils
	echo "WILL RUN"
	./app_TSP-fcore-ils

fcore-brkga:
	echo "WILL JOIN FILES INTO COMPLETE EXAMPLE"
	./join.sh
	echo "WILL COMPILE"
	g++ -g -O3 --std=c++20 -I../../include  mainTSP-fcore-brkga.cpp -o app_TSP-fcore-brkga
	echo "WILL RUN"
	./app_TSP-fcore-brkga

run:
	@echo "Running FCore (c++17)"
	./app_TSP-fcore-ils 
	./app_TSP-fcore-brkga 
	
