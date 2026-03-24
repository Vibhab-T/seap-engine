CXX = g++
CXXFLAGS = -std=c++23 -Wall -Isrc -Ilib/imgui -Ilib/imgui-sfml
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lGL

SRC_DIR = src
LIB_DIR = lib
BUILD_DIR = bin/build
OBJ_DIR = $(BUILD_DIR)/obj

TARGET = $(BUILD_DIR)/game

SRC = $(SRC_DIR)/main.cpp \
      $(SRC_DIR)/Game.cpp \
      $(SRC_DIR)/Entity.cpp \
      $(SRC_DIR)/EntityManager.cpp \
      $(SRC_DIR)/Vec2.cpp \
      $(LIB_DIR)/imgui/imgui.cpp \
      $(LIB_DIR)/imgui/imgui_draw.cpp \
      $(LIB_DIR)/imgui/imgui_tables.cpp \
      $(LIB_DIR)/imgui/imgui_widgets.cpp \
      $(LIB_DIR)/imgui-sfml/imgui-SFML.cpp

OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(filter $(SRC_DIR)/%,$(SRC))) \
      $(patsubst $(LIB_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(filter $(LIB_DIR)/%,$(SRC)))

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	@echo "Running game..."
	@./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean run