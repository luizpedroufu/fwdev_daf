TARGET = app
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

SRCS = main.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c app_sm.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

run: $(TARGET)
	./$(TARGET)
