
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
	g++-10.1 --coverage -g -O0 -fmax-errors=5 -std=c++20 -I$(SRC_PATH) -I$(LIBS_PATH) -DCATCH_CONFIG_MAIN $< -o ./build/$@   

test-examples:
	./test-examples.sh

test-coverage:
	mkdir -p reports
	lcov --directory . --capture --output-file reports/app.info
	(cd reports && genhtml app.info)

# cleaning tests
clean:
	rm -f *.test
	rm -f *_test
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.info
