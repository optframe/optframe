
all: test
	@echo "Executing tests"
	./build/app_tests

test: clean app_tests 
	

app_tests : tests.cpp
	mkdir -p build
	g++ --coverage -fsanitize=address -g -O0 -Wfatal-errors -fmax-errors=5 -std=c++20 -I../src -I../libs -I../../../src -DCATCH_CONFIG_MAIN $< -o ./build/$@   


# cleaning tests
clean:
	rm -f *.test
	rm -f *_test
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.info
