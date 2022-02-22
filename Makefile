SRC_DIR := ./src
OBJ_DIR := ./object
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := 
CXXFLAGS := -Wall -pedantic

solver: $(OBJ_FILES)
	g++ $(LDFLAGS) -o $@ $^

run: solver
	@echo "-----------------START-----------------\n"
	@./solver
	@echo "\n------------------END------------------"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
