
ROOT_PATH=..
SRC_PATH=../src
LIBS_PATH=../libs

all: test
	@echo "Executing tests"
	./build/OptFrame_test

test: clean OptFrame_test 
	

OptFrame_test : OptFrame.Test.cpp
	g++ --coverage -g -O0 -fconcepts -fmax-errors=5 -std=c++17 -I$(SRC_PATH) -I$(LIBS_PATH) -DCATCH_CONFIG_MAIN $< -o ./build/$@   

FCore_test : FCore.Test.cpp
	g++-10.1 --coverage -g -O0 -fmax-errors=5 -std=c++20 -fcoroutines -I$(SRC_PATH) -I$(LIBS_PATH) -DCATCH_CONFIG_MAIN $< -o ./build/$@   

test-install:
	g++ -g --std=c++17 -fconcepts test-install-OptFrame.cpp -o ./build/optframe_test_install
	./build/optframe_test_install
	g++-10.1 -g --std=c++20 -fcoroutines test-install-OptFCore.cpp -o ./build/fcore_test_install
	./build/fcore_test_install

CXXFLAGS_BCH=-std=c++20 -O3 -Wall -fcoroutines -fno-exceptions -fno-rtti -fno-omit-frame-pointer #  -pedantic -Werror
GBENCH_DIRS=-isystem ../libs/gbenchmark/include -L../libs/gbenchmark/build/src
GBENCH_LIBS=-lbenchmark 

bench: FCore.bench.cpp
	g++-10.1 $< $(CXXFLAGS_BCH) -I../src -I../Examples $(GBENCH_DIRS) -o build/FCore_bench $(GBENCH_LIBS) -lpthread

#PERF=./perf-4.19.0/tools/perf/perf
PERF=linux-4.16.12/tools/perf/perf


#######################

pair: pair17 pair20

pair17 : concept-test/Pair.cpp
	g++-8      -fconcepts -std=c++17 $< -o pair17   
pair20 : concept-test/Pair.cpp
	g++-10.1            -std=c++20 $< -o pair20  -fconcepts-diagnostics-depth=2

.PHONY: pair17 pair20 clean

####################

test-examples:
	./test-examples.sh

test-coverage:
	mkdir -p reports
	lcov --directory . --capture --output-file reports/app.info
	(cd reports && genhtml app.info)


deps:
    # export CXX=/usr/bin/clang++ && make deps
	./get_gbenchmark.sh

# cleaning tests
clean:
	rm -f *.test
	rm -f *_test
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.info
