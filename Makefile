CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -std=c11
TARGET := oop-in-c-demo
SOURCES := main.c animal.c dog.c cat.c

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES) animal.h dog.h cat.h
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
