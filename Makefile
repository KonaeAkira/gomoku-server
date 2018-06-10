SRC_DIR := src/http
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -lboost_system -lboost_thread -lpthread
CPPFLAGS := -std=c++11 -O2

.PHONY: gomoku-server.out clean

gomoku-server.out: $(OBJ_FILES)
	g++ $(CPPFLAGS) src/main.cpp $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) -c -o $@ $<
	
clean :
	-rm gomoku-server.out $(OBJ_FILES)
