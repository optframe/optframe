MAINCPP := mainEternityII.cpp
APPNAME := app_EtII

all: build/$(APPNAME)

CC=g++
CXXFLAGS= -I../../src  -fno-exceptions -O3 -Wall --std=c++17 -fconcepts #-fno-exceptions -fno-rtti #TODO: disable exceptions
LDFLAGS := $(CXXFLAGS)

SRC_DIR := ./src
OBJ_DIR := ./build
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)  $(MAINCPP)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

build/$(APPNAME): $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CXXFLAGS) -c -o $@ $<

.PHONY: all clean

clean: 
	rm -f build/$(APPNAME)
	rm -f build/*.o
	