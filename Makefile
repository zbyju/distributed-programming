SRC_DIR := ./src
OBJ_DIR := ./object
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := 
CXXFLAGS := -Wall -pedantic -std=c++14

all: folder solver

solver: $(OBJ_FILES)
	g++ $(LDFLAGS) -o $@ $^

run: all
	@echo "-----------------START-----------------\n"
	@./solver -n 10 -k 2 -f test.txt
	@echo "\n------------------END------------------"

folder:
	mkdir -p object

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f solver
	rm -rf ./object
