CXX = g++
CXXFLAGS = -std=c++17 -Wall -g
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# List of source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# List of object files
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Target executable
TARGET = main

# Main rule to build the target executable
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# Rule to compile each source file into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I $(INCLUDE_DIR)

# Create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to clean up generated files
clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean
