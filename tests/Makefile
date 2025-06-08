
ROOT_PATH=..
INC_PATH=../include
LIBS_PATH=../libs
CATCH2_MAIN_PART=-Ithirdparty -DCATCH_CONFIG_MAIN thirdparty/catch2/catch_amalgamated.cpp -DMAKE

all: test
	@echo "Executing tests"
	./build/OptFrame_test

test: clean test_lib OptFrame_test 

test_lib: copy_txt_lib_instances test_lib_base test_lib_base_cpp17
	(cd build && ./test_lib_base)
	(cd build && ./test_lib_base_cpp17)

copy_txt_lib_instances:
	mkdir -p build
	cp ./OptFrameLib-test/*.txt ./build/

test_lib_base: OptFrameLib-test/optframelib_base.Test.cpp
	g++ -g -O0 -std=c++20 -Wfatal-errors -pedantic -fsanitize=address -I$(INC_PATH) -I$(LIBS_PATH) -Ithirdparty ../src/OptFrameLib/OptFrameLib.cpp $<  -o ./build/$@   
	
test_lib_base_cpp17: OptFrameLib-test/optframelib_base_cpp17.Test.cpp
	g++ -g -O0 -std=c++17 -Wfatal-errors -pedantic -fsanitize=address -I$(INC_PATH) -I$(LIBS_PATH) -Ithirdparty ../src/OptFrameLib/OptFrameLib.cpp $<  -o ./build/$@ -DMOVE_IS_COMPONENT

test_lib_long: copy_txt_lib_instances test_lib_base test_lib_tsp_clean test_lib_kp test_lib_tsp
	(cd build && ./test_lib_base)
	(cd build && ./test_lib_tsp_clean)
	(cd build && ./test_lib_kp)
	(cd build && ./test_lib_tsp)

test_lib_kp: OptFrameLib-test/optframelib_kp.Test.cpp
	g++ -g -O3 -std=c++20 -pedantic  -I$(INC_PATH) -I$(LIBS_PATH) -Ithirdparty ../src/OptFrameLib/OptFrameLib.cpp $<  -o ./build/$@   

test_lib_tsp: OptFrameLib-test/optframelib_tsp.Test.cpp
	g++ -g -O3 -std=c++20 -pedantic -I$(INC_PATH) -I$(LIBS_PATH) -Ithirdparty ../src/OptFrameLib/OptFrameLib.cpp $<  -o ./build/$@   

test_lib_tsp_clean: OptFrameLib-test/optframelib_tsp_clean.cpp
	g++ -g -O3 -Wfatal-errors -std=c++20 -pedantic -fsanitize=address -I$(INC_PATH) -I$(LIBS_PATH) -Ithirdparty ../src/OptFrameLib/OptFrameLib.cpp $<  -o ./build/$@   
	

OptFrame_test : OptFrame.Test.cpp 
	g++ --coverage -g -O0 -fmax-errors=5 -std=c++20 -pedantic -I$(INC_PATH) -I$(LIBS_PATH) $(CATCH2_MAIN_PART) $<  -o ./build/$@   

FCore_test : FCore.Test.cpp
	g++-10.1 --coverage -g -O0 -fmax-errors=5 -std=c++20 -fcoroutines -I$(INC_PATH) -I$(LIBS_PATH) $(CATCH2_MAIN_PART) $< -o ./build/$@   

test-install:
	g++ -g --std=c++20 test-install-OptFrame.cpp -o ./build/optframe_test_install
	./build/optframe_test_install
	g++-10.1 -g --std=c++20 -fcoroutines test-install-OptFCore.cpp -o ./build/fcore_test_install
	./build/fcore_test_install

# -stdlib=libc++ on clang would require rebuilding google-benchmark
CXXFLAGS_BCH=-std=c++20   -O3 -Wall -DCORO  -fcoroutines     -fno-exceptions -fno-rtti -fno-omit-frame-pointer #  -pedantic -Werror
CLANGFLAGS_BCH=-std=c++20 -O3 -Wall         -fcoroutines-ts  -fno-exceptions -fno-rtti -fno-omit-frame-pointer #  -pedantic -Werror
GBENCH_DIRS=-isystem ../libs/gbenchmark/include -L../libs/gbenchmark/build/src
GBENCH_LIBS=-lbenchmark 

bench: FCore.bench.cpp
	g++-10.1 $< $(CXXFLAGS_BCH) -I../src -I../Examples $(GBENCH_DIRS) -o build/FCore_bench $(GBENCH_LIBS) -lpthread

bench_alt: AltFrame.bench.cpp
	#sudo cpupower frequency-set --governor performance
	g++-10.1 -g $< $(CXXFLAGS_BCH) -I../src  $(GBENCH_DIRS) -o build/AltFrame_bench $(GBENCH_LIBS) -lpthread
	#clang++-11  -g $< $(CLANGFLAGS_BCH) -I../src  $(GBENCH_DIRS) -o build/AltFrame_bench $(GBENCH_LIBS) -lpthread
	./build/AltFrame_bench --benchmark_filter=TSP_AltFrame
	#valgrind ./build/AltFrame_bench --benchmark_filter=TSP_AltFrame_NSSeqTestPtr_raw_coro
	#sudo cpupower frequency-set --governor powersave

bench_perf:
	#enable high performance 
	sudo cpupower frequency-set --governor performance
	# sudo cpupower frequency-set --governor powersave

#PERF=./perf-4.19.0/tools/perf/perf
PERF=linux-4.16.12/tools/perf/perf


#######################

pair: pair17 pair20

pair17 : concept-test/Pair.cpp
	g++-8               -std=c++17 $< -o pair17   
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
