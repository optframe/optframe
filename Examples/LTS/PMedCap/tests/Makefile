
MY_PATH=../src                # My project folder
SRC_PATH=../../../include     # OptFrame Source
LIBS_PATH=../../../libs   # OptFrame/Examples Libs

all: test
	@echo "Executing tests"
	./build/Example_test

test: build/Example_test
	
# the wildcards just track .h and .cpp for changes (avoiding 'clean')
build/Example_test : Example.Test.cpp $(wildcard $(MY_PATH)/*.h) $(wildcard $(MY_PATH)/*.cpp)
	g++ --coverage -DMAKE -g -O0 --std=c++20  -I$(MY_PATH) -I$(SRC_PATH) -I$(LIBS_PATH) -DCATCH_CONFIG_MAIN -o $@ $< 

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
