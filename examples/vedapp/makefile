CC = gcc
CFLAGS = -Wall

APP_DIR = ./
LIB_DIR = ../../source

SRCS = $(wildcard *.c) $(wildcard $(APP_DIR)/*.c) $(wildcard $(LIB_DIR)/*.c)
OBJS = $(SRCS:.c=.o)

TARGET = vedapp

INC_DIRS = -I$(APP_DIR) -I$(LIB_DIR)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INC_DIRS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
