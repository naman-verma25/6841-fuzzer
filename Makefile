# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address

# Directories
TARGET_DIR = targets
VIEWER_DIR = viewer

# Targets
TARGET_SOURCES = $(wildcard $(TARGET_DIR)/*.c)
TARGET_BINS = $(patsubst $(TARGET_DIR)/%.c,$(TARGET_DIR)/%,$(TARGET_SOURCES))

# Fuzzer sources and objects
FUZZER_SRC = fuzzer.c
FUZZER_OBJS = fuzzer.o $(VIEWER_DIR)/html_report.o
FUZZER_BIN = fuzzer

.PHONY: all clean

all: $(FUZZER_BIN) $(TARGET_BINS)

# Build fuzzer binary
$(FUZZER_BIN): $(FUZZER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile fuzzer.o
fuzzer.o: $(FUZZER_SRC) $(VIEWER_DIR)/html_report.h
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile viewer/html_report.o
$(VIEWER_DIR)/html_report.o: $(VIEWER_DIR)/html_report.c $(VIEWER_DIR)/html_report.h
	$(CC) $(CFLAGS) -c -o $@ $<

# Build targets binaries
$(TARGET_DIR)/%: $(TARGET_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f fuzzer.o $(VIEWER_DIR)/html_report.o $(FUZZER_BIN)
	rm -f $(TARGET_BINS)
