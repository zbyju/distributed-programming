SRC_DIR := ./src
OBJ_DIR := ./object
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -fsanitize=address -g
CXXFLAGS := -Wall -pedantic -std=c++14 -fsanitize=address

all: folder solver

solver: $(OBJ_FILES)
	g++ $(LDFLAGS) -o $@ $^

run: all
	@echo "-----------------START-----------------\n"
	@MallocNanoZone=0 ./solver -f ./inputs/graf_15_8.txt
	@echo "\n------------------END------------------"

folder:
	mkdir -p object

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f solver
	rm -rf ./object

runAll: all 10_3 10_5 10_6 10_7 12_3 12_5 12_6 12_9 15_4 15_5 15_6 15_8
runFast: all 10_3 10_5 10_6 10_7 12_3 12_5 12_6  15_4 15_5 15_6
runSlow: all 12_9 15_8

10_3:
	@MallocNanoZone=0 ./solver -f ./inputs/graf_$@.txt
10_5:
	@MallocNanoZone=0 ./solver -f ./inputs/graf_$@.txt
10_6:
	@MallocNanoZone=0 ./solver -f ./inputs/graf_$@.txt
10_7:
	@MallocNanoZone=0 ./solver -f ./inputs/graf_$@.txt
12_3:
	@MallocNanoZone=0 ./solver -f ./inputs/graf_$@.txt
12_5:
	@MallocNanoZone=0 ./solver -f ./inputs/graf_$@.txt
12_6:
	@MallocNanoZone=0 ./solver -f ./inputs/graf_$@.txt
12_9:
	@MallocNanoZone=0 ./solver -f ./inputs/graf_$@.txt
15_4:
	@MallocNanoZone=0 ./solver -f ./inputs/graf_$@.txt
15_5:
	@MallocNanoZone=0 ./solver -f ./inputs/graf_$@.txt
15_6:
	@MallocNanoZone=0 ./solver -f ./inputs/graf_$@.txt
15_8:
	@MallocNanoZone=0 ./solver -f ./inputs/graf_$@.txt
