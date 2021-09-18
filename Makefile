CC = gcc

INCLUDE_PATHS=$(shell pwd)
INCLUDES=$(INCLUDE_PATHS:%=-I%)

# The CFLAGS variable sets compile flags for gcc:
#  -g          compile with debug information
#  -Wall       give verbose compiler warnings
CFLAGS += -g -Wall $(INCLUDES)

MODULE_DIR := src

SOURCES = $(MODULE_DIR)/$(TARGET).c
BUILD_TARGET = $(MODULE_DIR)/$(TARGET)


all: $(TARGET)

build:
	$(CC) -o $(BUILD_TARGET) $(SOURCES) $(CFLAGS)

.PHONY: clean watch

clean:
	rm -f $(BUILD_TARGET) $(OBJECTS) core

watch:
	while true; do \
		inotifywait -qr -e modify -e create -e delete -e move $(SOURCES); \
		make build; \
		./$(BUILD_TARGET); \
	done