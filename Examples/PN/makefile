all: build/app_Example

CC=g++
CXXFLAGS= -I../../include -I../../libs -Iinclude  -fno-exceptions -O3 -Wall --std=c++20 #-fno-exceptions -fno-rtti #TODO: disable exceptions
LDFLAGS := $(CXXFLAGS)

SRC_DIR := ./src
OBJ_DIR := ./build
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)   # list source files
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))  # replace src/ by build/ on objs

# pack all objs ($^) into a single APPNAME
build/app_Example: $(OBJ_FILES)
	$(CC) $(LDFLAGS)  -o $@ $^

# compile each .cpp individually ($<)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CXXFLAGS) -c -o $@ $<

.PHONY: all clean

clean: 
	rm -f build/app_*
	rm -f build/*.o
	
test:
	cd tests && make

test-coverage:
	cd tests && make test-coverage
