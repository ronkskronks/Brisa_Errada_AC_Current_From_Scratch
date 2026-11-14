CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Iinclude -O2
# NOTE: No -lm flag is permitted. All math is implemented from scratch.

BUILD_DIR = build
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests

# Module Object files
MATH_OBJ  = $(BUILD_DIR)/math_engine.o
ELEC_OBJ  = $(BUILD_DIR)/electrical.o
WAVE_OBJ  = $(BUILD_DIR)/waveform.o
GRAPH_OBJ = $(BUILD_DIR)/graph.o

ALL_OBJS = $(MATH_OBJ) $(ELEC_OBJ) $(WAVE_OBJ) $(GRAPH_OBJ)

# Test Binaries
TEST_MATH_BIN = $(BUILD_DIR)/test_math
TEST_ELEC_BIN = $(BUILD_DIR)/test_electrical
TEST_WAVE_BIN = $(BUILD_DIR)/test_waveform

# Main Application Binary
MAIN_BIN = $(BUILD_DIR)/ac_analyzer
ROOT_BIN = ac_analyzer

.PHONY: all clean test test_math test_electrical test_waveform run dirs

all: dirs $(MAIN_BIN) $(ROOT_BIN) test

dirs:
	@mkdir -p $(BUILD_DIR)

# Object compilation rules
$(MATH_OBJ): $(SRC_DIR)/math_engine.c $(INC_DIR)/math_engine.h | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(ELEC_OBJ): $(SRC_DIR)/electrical.c $(INC_DIR)/electrical.h $(INC_DIR)/math_engine.h | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(WAVE_OBJ): $(SRC_DIR)/waveform.c $(INC_DIR)/waveform.h $(INC_DIR)/math_engine.h | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(GRAPH_OBJ): $(SRC_DIR)/graph.c $(INC_DIR)/graph.h $(INC_DIR)/waveform.h $(INC_DIR)/math_engine.h | dirs
	$(CC) $(CFLAGS) -c $< -o $@

# Main Application Binary
$(MAIN_BIN): $(SRC_DIR)/main.c $(ALL_OBJS) | dirs
	$(CC) $(CFLAGS) $^ -o $@

$(ROOT_BIN): $(MAIN_BIN)
	cp $(MAIN_BIN) $(ROOT_BIN)

# Test Binaries
$(TEST_MATH_BIN): $(TEST_DIR)/test_math.c $(MATH_OBJ) | dirs
	$(CC) $(CFLAGS) $^ -o $@

$(TEST_ELEC_BIN): $(TEST_DIR)/test_electrical.c $(ELEC_OBJ) $(MATH_OBJ) | dirs
	$(CC) $(CFLAGS) $^ -o $@

$(TEST_WAVE_BIN): $(TEST_DIR)/test_waveform.c $(WAVE_OBJ) $(GRAPH_OBJ) $(MATH_OBJ) | dirs
	$(CC) $(CFLAGS) $^ -o $@

# Test execution targets
test_math: $(TEST_MATH_BIN)
	@echo "--- Running Mathematical Engine Tests ---"
	@./$(TEST_MATH_BIN)

test_electrical: $(TEST_ELEC_BIN)
	@echo "--- Running Electrical Calculations Tests ---"
	@./$(TEST_ELEC_BIN)

test_waveform: $(TEST_WAVE_BIN)
	@echo "--- Running Waveform & Plotting Tests ---"
	@./$(TEST_WAVE_BIN)

test: test_math test_electrical test_waveform

run: $(ROOT_BIN)
	./$(ROOT_BIN)

clean:
	rm -rf $(BUILD_DIR) $(ROOT_BIN)
