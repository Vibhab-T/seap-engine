CXX = g++
CXXFLAGS = -std=c++23 -Wall -Isrc
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

SRC_DIR = src
BUILD_DIR = bin/build
OBJ_DIR = $(BUILD_DIR)/obj

TARGET = $(BUILD_DIR)/game

SRC = $(SRC_DIR)/main.cpp \
      $(SRC_DIR)/Game.cpp \
      $(SRC_DIR)/Entity.cpp \
      $(SRC_DIR)/EntityManager.cpp \
      $(SRC_DIR)/Vec2.cpp

OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	@echo "Running game..."
	@./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean run