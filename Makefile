CC        = clang
CFLAGS    = -Wall -O0 -g -o
SRC_DIR   = src
SRC       = $(SRC_DIR)/main.c
BUILD_DIR = build
TARGET    = $(BUILD_DIR)/main

.PHONY: $(TARGET)
$(TARGET): $(SRC)
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $@ $<

.PHONY: run
run: $(TARGET)
	@$(TARGET)

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)
