# Makefile for building quiz

CC = gcc
CFLAGS = -s -no-pie -fno-omit-frame-pointer
LDFLAGS = -lcrypto
TARGET = quiz
SRC = quiz.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

