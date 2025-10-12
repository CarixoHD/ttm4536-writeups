CC := gcc
CFLAGS := -Wall -Wextra -fno-stack-protector \
          -Wno-unused-result
TARGET := todo
SRC := main.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
