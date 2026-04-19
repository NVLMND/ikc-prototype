CC = gcc
CFLAGS = -g -Wall -Wextra
TARGET = ikc
all:
	$(CC) $(CFLAGS) *.c -o $(TARGET)

