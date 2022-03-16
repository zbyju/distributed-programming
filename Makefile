SRC_DIR := ./src
OBJ_DIR := ./object
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -fsanitize=address -fopenmp -g
CXXFLAGS := -Wall -pedantic -std=c++14 -fsanitize=address -fopenmp

all: folder solver

starSeq:
	g++ -std=c++14 ./src/main_seq.cpp -o solver_seq

starTask:
	g++ -std=c++14 -fopenmp ./src/main_task.cpp -o solver_task	

starRunSeq: starSeq 
	cleanStar
	qrun 20c 1 pdp_serial ./star_scripts/run_seq.txt
	cat run_seq.txt.o*

starRunTask: starTask
	cleanStar
	qrun 20c 1 pdp_serial ./star_scripts/run_task_par.txt
	cat run_task_par.txt.o*

starRunComp: starSeq starTask
	cleanStar
	qrun 20c 1 pdp_serial ./star_scripts/task_seq_comparison.txt
	cat task_seq_comparison.txt.o*

solver: $(OBJ_FILES)
	g++ $(LDFLAGS) -o $@ $^

run: all
	@echo "-----------------START-----------------\n"
	@MallocNanoZone=0 ./solver -f ./inputs/graf_15_6.txt
	@echo "\n------------------END------------------"

folder:
	mkdir -p object

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f solver
	rm -rf ./object

cleanStar:
	rm -f solver
	rm -f solver_seq
	rm -f solver_task
	rm -f ./*.txt.*

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
