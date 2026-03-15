CXX = g++
CXXFLAGS = -std=c++23 -Wall -Iinclude -Ilib/imgui -Ilib/imgui-sfml
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lGL

SRC = src/main.cpp \
      lib/imgui/imgui.cpp \
      lib/imgui/imgui_draw.cpp \
      lib/imgui/imgui_tables.cpp \
      lib/imgui/imgui_widgets.cpp \
      lib/imgui/misc/cpp/imgui_stdlib.cpp \
      lib/imgui-sfml/imgui-SFML.cpp

BUILD_DIR = bin/build
OBJ_DIR = $(BUILD_DIR)/obj

# Map source files to object files in OBJ_DIR
OBJ = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

TARGET = $(BUILD_DIR)/my_app

all: $(TARGET)

# Build target in bin/build
$(TARGET): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# Compile each object file to OBJ_DIR
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean all object files and target
clean:
	rm -rf $(BUILD_DIR)

# Run target: clean, build, cd to build, run, return
run: clean
	$(MAKE) $(TARGET)
	@echo "Running $(TARGET)..."
	@cd $(BUILD_DIR) && ./my_app
	@echo "Returned to source folder."