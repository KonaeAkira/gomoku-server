SRC_DIR := src
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -lboost_system -lboost_thread -lpthread
CXXFLAGS := -std=c++11 -O2

.PHONY: clean

gomoku-server.out: $(OBJ_FILES)
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	-mkdir -p obj
	g++ $(CXXFLAGS) -c -o $@ $<
	
clean:
	-rm $(OBJ_FILES)
