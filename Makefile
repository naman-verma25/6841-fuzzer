# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address
TARGET_DIR = targets

# Target binaries
TARGETS = buffer format_strings heap_overflow heap_use_after_free
TARGET_BINS = $(addprefix $(TARGET_DIR)/, $(TARGETS))

# Fuzzer
FUZZER = fuzzer
FUZZER_SRC = fuzzer.c

.PHONY: all clean

all: $(FUZZER) $(TARGET_BINS)

$(FUZZER): $(FUZZER_SRC)
	$(CC) $(CFLAGS) -o $(FUZZER) $(FUZZER_SRC)

$(TARGET_DIR)/%: %.c | $(TARGET_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

clean:
	rm -f $(FUZZER) $(TARGET_BINS)
	rm -rf $(TARGET_DIR)
