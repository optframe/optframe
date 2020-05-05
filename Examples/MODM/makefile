#CXX = g++-8
CXX = g++-7
CPPFLAGS = -I../../src  -fno-exceptions -O3
CPPFLAGS += -MD -MP --std=c++17 -fconcepts #-fno-exceptions -fno-rtti #TODO: disable exceptions


all: app_MODM

SRC_MODM = $(wildcard MODM/*.cpp) mainMODM.cpp

app_MODM: $(SRC_MODM:%.cpp=%.o)
	$(CXX) -o $@ $^

-include $(SRC_MODM:%.cpp=%.d)


clean: 
	rm -f app_*
	rm -f *.o *.d
	rm -f MODM/*.o MODM/*.d
	rm -f src/*.o src/*.d
