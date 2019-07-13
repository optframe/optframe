FILENAME :=  $(patsubst %.Test.cpp,%.test,$(wildcard *.Test.cpp))

all: $(FILENAME)
	@echo "Executing test for: $(FILENAME)"
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:gtestlib/googlemock:gtestlib/googlemock/gtest && ./$(FILENAME)

# compiling test
%.test : %.Test.cpp
	g++ -Ofast --std=c++11 --coverage -Igtestlib/googletest/include -Lgtestlib/googlemock/gtest -Lgtestlib/googlemock -lgtest -lgtest_main $< -o $@

# cleaning tests
clean:
	rm -f *.test	
	rm -f *.gcda
	rm -f *.gcno	

