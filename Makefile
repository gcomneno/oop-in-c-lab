CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -std=c11
TARGET := oop-in-c-demo
CONTAINER_OF_TARGET := container-of-demo
OPAQUE_TARGET := opaque-demo
SOURCES := main.c animal.c dog.c cat.c
CONTAINER_OF_SOURCES := examples/container_of_demo.c
OPAQUE_DIR := examples/opaque
OPAQUE_SOURCES := $(OPAQUE_DIR)/opaque_demo.c \
                  $(OPAQUE_DIR)/opaque_animal.c \
                  $(OPAQUE_DIR)/opaque_dog.c \
                  $(OPAQUE_DIR)/opaque_cat.c

.PHONY: all run run-container-of run-opaque clean

all: $(TARGET)

$(TARGET): $(SOURCES) animal.h dog.h cat.h
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

run: all
	./$(TARGET)

$(CONTAINER_OF_TARGET): $(CONTAINER_OF_SOURCES) animal.h
	$(CC) $(CFLAGS) $(CONTAINER_OF_SOURCES) -o $(CONTAINER_OF_TARGET)

run-container-of: $(CONTAINER_OF_TARGET)
	./$(CONTAINER_OF_TARGET)

$(OPAQUE_TARGET): $(OPAQUE_SOURCES) \
                  $(OPAQUE_DIR)/opaque_animal.h \
                  $(OPAQUE_DIR)/opaque_animal_internal.h \
                  $(OPAQUE_DIR)/opaque_dog.h \
                  $(OPAQUE_DIR)/opaque_cat.h
	$(CC) $(CFLAGS) -I$(OPAQUE_DIR) $(OPAQUE_SOURCES) -o $(OPAQUE_TARGET)

run-opaque: $(OPAQUE_TARGET)
	./$(OPAQUE_TARGET)

clean:
	rm -f $(TARGET) $(CONTAINER_OF_TARGET) $(OPAQUE_TARGET)
