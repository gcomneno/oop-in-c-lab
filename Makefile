CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -std=c11
TARGET := oop-in-c-demo
CONTAINER_OF_TARGET := container-of-demo
SOURCES := main.c animal.c dog.c cat.c
CONTAINER_OF_SOURCES := examples/container_of_demo.c

.PHONY: all run run-container-of clean

all: $(TARGET)

$(TARGET): $(SOURCES) animal.h dog.h cat.h
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

run: all
	./$(TARGET)

$(CONTAINER_OF_TARGET): $(CONTAINER_OF_SOURCES) animal.h
	$(CC) $(CFLAGS) $(CONTAINER_OF_SOURCES) -o $(CONTAINER_OF_TARGET)

run-container-of: $(CONTAINER_OF_TARGET)
	./$(CONTAINER_OF_TARGET)

clean:
	rm -f $(TARGET) $(CONTAINER_OF_TARGET)
