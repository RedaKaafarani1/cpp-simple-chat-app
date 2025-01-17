# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Directories
SERVER_SRC_DIR = src/server
CLIENT_SRC_DIR = src/client
BUILD_DIR = build

# Source and object files
SERVER_SRCS = $(wildcard $(SERVER_SRC_DIR)/*.cpp)
CLIENT_SRCS = $(wildcard $(CLIENT_SRC_DIR)/*.cpp)
SERVER_OBJS = $(SERVER_SRCS:%.cpp=$(BUILD_DIR)/%.o)
CLIENT_OBJS = $(CLIENT_SRCS:%.cpp=$(BUILD_DIR)/%.o)

# Output binaries
SERVER_BIN = server
CLIENT_BIN = client

# Default target
all: $(SERVER_BIN) $(CLIENT_BIN)

# Build server binary
$(SERVER_BIN): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build client binary
$(CLIENT_BIN): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(SERVER_BIN) $(CLIENT_BIN)

# Run the server
run-server: $(SERVER_BIN)
	./$(SERVER_BIN)

# Run the client
run-client: $(CLIENT_BIN)
	./$(CLIENT_BIN)

.PHONY: all clean run-server run-client
