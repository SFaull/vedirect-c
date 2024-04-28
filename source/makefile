CC = gcc
CFLAGS = -Wall

PROJECT_DIR = ./

SRCS = $(wildcard *.c) $(wildcard $(PROJECT_DIR)/*.c)
OBJS = $(SRCS:.c=.o)

TARGET = vedparse

INC_DIRS = -I$(PROJECT_DIR)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INC_DIRS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
