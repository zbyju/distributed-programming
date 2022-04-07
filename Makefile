SRC_DIR := ./src
OBJ_DIR := ./object
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -fsanitize=address -fopenmp -g
CXXFLAGS := -Wall -pedantic -std=c++14 -fsanitize=address -fopenmp

INPUT := 15_6
PROCESSCOUNT = 5

all: folder solverSeq solverTask solverData solverMpi

solverSeq: ./src/main_seq.cpp
	g++ $(CXXFLAGS) ./src/main_seq.cpp -o solver_seq

solverTask: ./src/main_task.cpp
	g++ $(CXXFLAGS) ./src/main_task.cpp -o solver_task

solverData: ./src/main_data.cpp
	g++ $(CXXFLAGS) ./src/main_data.cpp -o solver_data

solverMpi:
	mpic++ $(CXXFLAGS) ./src/main_mpi.cpp -o solver_mpi

runMpi: solverMpi
	mpirun -np ${PROCESSCOUNT} ./solver_mpi -f ./inputs/graf_15_6.txt

run: all
	@echo "=================================== START ===================================\n"
	./solver_seq -f ./inputs/graf_${INPUT}.txt
	./solver_task -f ./inputs/graf_${INPUT}.txt
	./solver_data -f ./inputs/graf_${INPUT}.txt
	mpirun -np ${PROCESSCOUNT} ./solver_mpi -f ./inputs/graf_${INPUT}.txt
	@echo "=================================== END ==================================="

runSD: folder solverData solverSeq
	@echo "-----------------START-----------------\n"
	./solver_seq -f ./inputs/graf_${INPUT}.txt
	./solver_data -f ./inputs/graf_${INPUT}.txt
	@echo "\n------------------END------------------"

runData: folder solverData
	@echo "-----------------START-----------------\n"
	./solver_data -f ./inputs/graf_${INPUT}.txt
	@echo "\n------------------END------------------"

folder:
	mkdir -p object

clean:
	rm -f solver*
	rm -rf ./object

cleanStar:
	rm -f solver
	rm -f solver_seq
	rm -f solver_task
	rm -f ./*.txt.*
