SRC_DIR := src
OBJ_DIR := obj
OUT_DIR := bin

DEPS := $(wildcard $(SRC_DIR)/*.hpp)

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

LDFLAGS := -lboost_system -lboost_thread -lpthread
CXXFLAGS := -std=c++11 -O2

.PHONY: default clean

default:
	@echo Building gomoku-server...
	$(MAKE) $(OUT_DIR)/gomoku-server.out
	@echo Finished building to $@

$(OUT_DIR)/gomoku-server.out: $(OBJ_FILES) $(DEPS)
	-mkdir -p $(OUT_DIR)
	g++ $(CXXFLAGS) $(OBJ_FILES) $(LDFLAGS) -o $@
	cp -r $(SRC_DIR)/web $(OUT_DIR)/web
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	-mkdir -p $(OBJ_DIR)
	g++ $(CXXFLAGS) -c -o $@ $<
	
clean:
	-rm $(OBJ_FILES)
