
SRC_PATH=../src
LIBS_PATH=../libs
# gtest
GTEST_PATH=./gtestlib/build/googletest
GMOCK_PATH=./gtestlib/build/googlemock
GTEST_INCLUDE=./gtestlib/googletest/include
GMOCK_INCLUDE=./gtestlib/googlemock/include

all:
	@echo "please type 'make test'"

test: clean OptFrame.test
	@echo "Executing tests"
	./OptFrame.test


OptFrame.test : OptFrame.Test.cpp
	g++ --coverage -g -O0 --std=c++11 -I$(SRC_PATH)/OptFrame -I$(LIBS_PATH) -I$(GTEST_INCLUDE) -I$(GMOCK_INCLUDE) $< -o $@ $(GMOCK_PATH)/gtest/libgtest_main.a $(GMOCK_PATH)/gtest/libgtest.a $(GMOCK_PATH)/libgmock.a $(LIBS_PATH)/scannerpplib/Scanner.cpp -lpthread -ldl


test-coverage:
	mkdir -p reports
	lcov --directory . --capture --output-file reports/app.info
	(cd reports && genhtml app.info)

# cleaning tests
clean:
	rm -f *.test
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.info
