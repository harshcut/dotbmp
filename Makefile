CC        = clang
CFLAGS    = -Wall -O0 -g -o
SRC_DIR   = src
SRC       = $(SRC_DIR)/main.c
BUILD_DIR = build
TARGET    = $(BUILD_DIR)/main

all: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $@ $<

run: $(TARGET)
	@$(TARGET)

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all run clean
