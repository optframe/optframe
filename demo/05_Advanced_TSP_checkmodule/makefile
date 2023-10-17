all: app_applyupdate  app_cost

app_applyupdate: TSP-fcore-part5-applyupdate.hpp
	echo "WILL JOIN FILES INTO COMPLETE EXAMPLE"
	./join.sh
	mv TSP-fcore-applyupdate.hpp TSP-fcore.hpp
	echo "WILL COMPILE"
	g++ -g -O3 --std=c++17 -I../../include -Wfatal-errors  main_checkmodule.cpp -o app_check
	echo "WILL RUN"
	./app_check

app_cost: TSP-fcore-part5-cost.hpp
	echo "WILL JOIN FILES INTO COMPLETE EXAMPLE"
	./join.sh
	mv TSP-fcore-cost.hpp TSP-fcore.hpp
	echo "WILL COMPILE"
	g++ -g -O3 --std=c++17 -I../../include -Wfatal-errors  main_checkmodule.cpp -o app_check
	echo "WILL RUN"
	./app_check


run:
	@echo "Running FCore (c++17)"
	./app_check
	