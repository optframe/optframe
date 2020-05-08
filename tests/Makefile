
SRC_PATH=../src
LIBS_PATH=../libs
TP_PATH=./thirdparty

all: test
	@echo "Executing tests"
	./build/OptFrame_test
	./build/Examples_test

test: clean OptFrame_test Examples_test
	

OptFrame_test : OptFrame.Test.cpp
	g++ --coverage -g -O0 -fconcepts -std=c++17 -I$(SRC_PATH) -I$(LIBS_PATH) -I$(TP_PATH) -DCATCH_CONFIG_MAIN $< -o ./build/$@   

Examples_test : Examples.Test.cpp
	g++ --coverage -g -O0 -fconcepts -std=c++17 -I$(SRC_PATH) -I$(SRC_PATH)/../Examples -I$(LIBS_PATH) -I$(TP_PATH) -DCATCH_CONFIG_MAIN  $< -o ./build/$@


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
