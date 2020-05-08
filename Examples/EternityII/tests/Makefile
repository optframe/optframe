
MY_PATH=..                # My project folder
SRC_PATH=../../../src     # OptFrame Source
LIBS_PATH=../../../libs   # OptFrame/Examples Libs

all: test
	@echo "Executing tests"
	./build/Example_test

test: clean build/Example_test
	
build/Example_test : Example.Test.cpp
	g++ --coverage -g -O0 -fconcepts -std=c++17 -I$(MY_PATH) -I$(SRC_PATH) -I$(LIBS_PATH) -DCATCH_CONFIG_MAIN -o $@ $< 

test-coverage:
	mkdir -p reports
	lcov --directory . --capture --output-file reports/app.info
	(cd reports && genhtml app.info)

# cleaning tests
clean:
	rm -f *.test
	rm -f build/*_test
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.info
