CC=gcc
CFLAGS=-Werror -pedantic -g
SRC=main.c
TARGET=lgtv

all: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
