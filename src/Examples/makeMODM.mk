all: app_MODM

SRC_MODM = $(wildcard MODM/*.cpp) mainMODM.cpp

app_MODM: $(SRC_MODM:%.cpp=%.o) MODMScanner.o
	g++ -o $@ $^

MODMScanner.o: ../OptFrame/Scanner++/Scanner.cpp
	g++ -c $(CPPFLAGS) ../OptFrame/Scanner++/Scanner.cpp -o MODMScanner.o

-include $(SRC_MODM:%.cpp=%.d)

CPPFLAGS = -O3
CPPFLAGS += -MD -MP --std=c++17 -fconcepts #-fno-exceptions -fno-rtti #TODO: disable exceptions

	
clean: 
	make clean -C ../Examples/
	rm -f app_*
	rm -f *.o *.d
	rm -f MODM/*.o MODM/*.d
