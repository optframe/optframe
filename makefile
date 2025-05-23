CXX_GCC=g++
CXX_CLANG=clang++-19
#CXX=clang++
#CPPSTD=-std=c++20   # -fconcepts-diagnostics-depth=2  # -Wfatal-errors
CPPSTD=--std=c++17   # TODO REMOVE # -Wfatal-errors

all: optframe_lib_test   test    optframe_lib_test_gcc_clang19

OPTFRAME_ROOT=.

optframe_lib: ./src/OptFrameLib/OptFrameLib.cpp
	mkdir -p build/
	@echo "TEST BUILD LIBRARY WITH ${CXX}"
	$(CXX) --std=c++20 -g -I${OPTFRAME_ROOT}/include   -Wall -pedantic -O3  --shared  $< -o build/optframe_lib.so -fPIC

optframe_lib_gcc_clang19: ./src/OptFrameLib/OptFrameLib.cpp
	mkdir -p build/
	#
	@echo "BUILD WITH ${CC_GCC} (PART 1/2)"
	$(CXX_GCC)   $(CPPSTD) -g -I${OPTFRAME_ROOT}/include -Wall -pedantic -Ofast               --shared $<  -o build/optframe_lib.so -fPIC
	#
	@echo "BUILD WITH ${CC_CLANG} (PART 2/2)"
	$(CXX_CLANG) $(CPPSTD) -g -I${OPTFRAME_ROOT}/include -Wall -pedantic -O3 --stdlib=libc++  --shared $< -o build/optframe_lib.so -fPIC
	#
	# readelf -s build/optframe_lib.so | grep fcore
	ls -la build/optframe_lib.so

check: test demos

demos:
	cd demo && make

test:
	cd tests && make test && ./build/OptFrame_test

test-install:
	cd tests && make test-install

install:
	echo "installing OptFrame headers in /usr/local/include/"
	./install.sh && echo "Finished Installation"

lint:
	bazel run @hedron_compile_commands//:refresh_all
	bazel build //... --config clang-tidy

.PHONY: docs

docs:
	# apt-get install python3-sphinx
	# apt-get install texlive texlive-latex-extra latexmk texlive-science
	python3 -m pip install -r docs/requirements.txt
	cd docs && make clean && make html && make latexpdf

clean:
	cd Examples && make clean
	bazel clean --expunge

