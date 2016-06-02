CC=gcc
CFLAGS=-Werror -pedantic -g
HEADERS=main.h
SRC=main.c serial.c
TARGET=lgtv

all: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
