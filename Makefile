SRC_DIR := ./src
OBJ_DIR := ./object
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -fsanitize=address -fopenmp -g
CXXFLAGS := -Wall -pedantic -std=c++14 -fsanitize=address -fopenmp

all: folder solverSeq solverTask solverData

solverSeq:
	g++ $(CXXFLAGS) ./src/main_seq.cpp -o solver_seq

solverTask:
	g++ $(CXXFLAGS) ./src/main_task.cpp -o solver_task

solverData:
	g++ $(CXXFLAGS) ./src/main_data.cpp -o solver_data

run: all
	@echo "-----------------START-----------------\n"
	./solver_seq -f ./inputs/graf_15_6.txt
	./solver_task -f ./inputs/graf_15_6.txt
	./solver_data -f ./inputs/graf_15_6.txt
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
