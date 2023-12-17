CC = gcc
CFLAGS = -O3
TARGET = main
LIB_DIR = lib
SRCS = main.c schedulers.c task_management.c
HEADERS = cpusim.h schedulers.h task_management.h
VEC_LIB = $(LIB_DIR)/libssvector

BINS = main $(VEC_LIB).o
CURRENT_DIR = $(shell pwd)

.PHONEY = clean all debug run

all: main

$(VEC_LIB).o: $(LIB_DIR)/ssvector.h $(VEC_LIB).c
	$(CC) -c $(CFLAGS) -o $@ $(VEC_LIB).c

main: $(HEADERS) $(SRCS) $(VEC_LIB).o
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(VEC_LIB).o

# Compile for gdb
debug: $(HEADERS) $(SRCS) $(VEC_LIB).o
	$(CC) -o $(TARGET) -g $(SRCS) $(VEC_LIB).o

run:
	./$(TARGET)

clean:
	rm -f $(BINS)
