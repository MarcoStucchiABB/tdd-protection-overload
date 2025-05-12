# Compiler for ARM and Windows
CC_WIN = gcc

# Directories
SRC_DIR = src
BUILD_DIR = build
TESTS_DIR = test
UNITY_DIR = tools/unity

# Files
SRCS = $(SRC_DIR)/protection_overload.c
TEST_SRCS = $(TESTS_DIR)/test_protection_overload.c
UNITY_SRC = $(UNITY_DIR)/unity.c

# Output Executables
OUT_WIN = $(BUILD_DIR)/test_protection_overload_win.exe

# Compiler Flags
CFLAGS = -I$(SRC_DIR) -I$(TESTS_DIR) -Wall -Wextra -std=c11
CFLAGS += -g
LDFLAGS_WIN = -lm  # No special specs needed for Windows

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build-only target
build_win: $(BUILD_DIR) $(OUT_WIN)

# Test targets (build + run)
test_win: build_win
	@echo "Running Windows tests..."
	$(OUT_WIN)

# Build both versions (ARM & Windows)
build_all: build_win

# Build & run both
test_all: test_win

# Clean build directory
clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.exe

# Windows Build (including mock sensor but excluding stubs)
$(OUT_WIN): $(SRCS) $(TEST_SRCS) $(UNITY_SRC)
	$(CC_WIN) $(CFLAGS) -I $(UNITY_DIR) -o $@ $^ $(LDFLAGS_WIN)


