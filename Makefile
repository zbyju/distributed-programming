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

starRunSeq: cleanStar starSeq 
	qrun 20c 1 pdp_serial ./star_scripts/run_seq.txt

starRunTask: cleanStar starTask
	qrun 20c 1 pdp_serial ./star_scripts/run_task_par.txt

starRunComp: cleanStar starSeq starTask
	qrun 20c 1 pdp_serial ./star_scripts/task_seq_comparison.txt

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

runAll: 10_3 10_5 10_6 10_7 12_3 12_5 12_6 12_9 15_4 15_5 15_6 15_8
runFast: 10_3 10_5 10_6 10_7 12_3 12_5 12_6  15_4 15_5 15_6
runFastSeq: 10_3_s 10_5_s 10_6_s 10_7_s 12_3_s 12_5_s 12_6_s  15_4_s 15_5_s 15_6_s
runSlow: 12_9 15_8

10_3:
	@MallocNanoZone=0 ./solver_task -f ./inputs/graf_$@.txt
10_5:
	@MallocNanoZone=0 ./solver_task -f ./inputs/graf_$@.txt
10_6:
	@MallocNanoZone=0 ./solver_task -f ./inputs/graf_$@.txt
10_7:
	@MallocNanoZone=0 ./solver_task -f ./inputs/graf_$@.txt
12_3:
	@MallocNanoZone=0 ./solver_task -f ./inputs/graf_$@.txt
12_5:
	@MallocNanoZone=0 ./solver_task -f ./inputs/graf_$@.txt
12_6:
	@MallocNanoZone=0 ./solver_task -f ./inputs/graf_$@.txt
12_9:
	@MallocNanoZone=0 ./solver_task -f ./inputs/graf_$@.txt
15_4:
	@MallocNanoZone=0 ./solver_task -f ./inputs/graf_$@.txt
15_5:
	@MallocNanoZone=0 ./solver_task -f ./inputs/graf_$@.txt
15_6:
	@MallocNanoZone=0 ./solver_task -f ./inputs/graf_$@.txt
15_8:
	@MallocNanoZone=0 ./solver_task -f ./inputs/graf_$@.txt


10_3_s:
	@MallocNanoZone=0 ./solver_seq -f ./inputs/graf_10_3.txt
10_5_s:
	@MallocNanoZone=0 ./solver_seq -f ./inputs/graf_10_5.txt
10_6_s:
	@MallocNanoZone=0 ./solver_seq -f ./inputs/graf_10_6.txt
10_7_s:
	@MallocNanoZone=0 ./solver_seq -f ./inputs/graf_10_7.txt
12_3_s:
	@MallocNanoZone=0 ./solver_seq -f ./inputs/graf_12_3.txt
12_5_s:
	@MallocNanoZone=0 ./solver_seq -f ./inputs/graf_12_5.txt
12_6_s:
	@MallocNanoZone=0 ./solver_seq -f ./inputs/graf_12_6.txt
12_9_s:
	@MallocNanoZone=0 ./solver_seq -f ./inputs/graf_12_9.txt
15_4_s:
	@MallocNanoZone=0 ./solver_seq -f ./inputs/graf_15_4.txt
15_5_s:
	@MallocNanoZone=0 ./solver_seq -f ./inputs/graf_15_5.txt
15_6_s:
	@MallocNanoZone=0 ./solver_seq -f ./inputs/graf_15_6.txt
15_8_s:
	@MallocNanoZone=0 ./solver_seq -f ./inputs/graf_15_8.txt
