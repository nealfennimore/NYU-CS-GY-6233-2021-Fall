CC = gcc

INCLUDE_PATHS=$(shell pwd)
INCLUDES=$(INCLUDE_PATHS:%=-I%)
DEFINE=DEBUG=1
DEFINES=$(DEFINE:%=-D%)

# The CFLAGS variable sets compile flags for gcc:
#  -g          compile with debug information
#  -Wall       give verbose compiler warnings
CFLAGS += -g -Wall $(INCLUDES) $(DEFINES)

SOURCES=$(shell find $$PWD/* -name '*.c' -not -path '*/01-intro-to-c/*')

BUILD_TARGET = main.o

all: $(BUILD_TARGET)

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