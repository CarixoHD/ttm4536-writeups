# Makefile for building licence-checker-h

CC = gcc
CFLAGS = -I. -s
TARGET = licence-checker
SRCS = licence-checker-h.c cdecode.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

