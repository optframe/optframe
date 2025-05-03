CC_GCC=g++
CC_CLANG=clang++-19
#CC=clang++
#CPPSTD=-std=c++20   # -fconcepts-diagnostics-depth=2  # -Wfatal-errors
CPPSTD=--std=c++17   # TODO REMOVE # -Wfatal-errors

all: optframe_lib_test   test    optframe_lib_test_gcc_clang19

OPTFRAME_C_LIB=./src/OptFrameLib/OptFrameLib.cpp
OPTFRAME_SRC=.

optframe_lib_test:
	@echo "TEST BUILD LIBRARY WITH ${CC}"
	$(CC) $(CPPSTD) -g -I${OPTFRAME_SRC}/include   -Wall -pedantic -Ofast               --shared tests/OptFrameLib-test/build_optframe_lib.cpp $(OPTFRAME_C_LIB)  -o build/optframe_lib.so -fPIC

optframe_lib_test_gcc_clang19:
	# mkdir -p build/
	# -Wextra
	#
	@echo "BUILD WITH ${CC_GCC} (PART 1/2)"
	$(CC_GCC) $(CPPSTD) -g -I${OPTFRAME_SRC}/include   -Wall -pedantic -Ofast               --shared tests/OptFrameLib-test/build_optframe_lib.cpp $(OPTFRAME_C_LIB)  -o build/optframe_lib.so -fPIC
	#
	@echo "BUILD WITH ${CC_CLANG} (PART 2/2)"
	$(CC_CLANG) $(CPPSTD) -g -I${OPTFRAME_SRC}/include -Wall -pedantic -O3 --stdlib=libc++  --shared tests/OptFrameLib-test/build_optframe_lib.cpp $(OPTFRAME_C_LIB) -o build/optframe_lib.so -fPIC
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

