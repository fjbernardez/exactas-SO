CC=gcc

ARCH := $(shell uname -m)

BINS=hai antikill

OBJS=$(BINS:=.o)

all: $(BINS)

# Dependencias
hai.o: hai.c
antikill.o: antikill.c

clean:
	rm -rf $(OBJS) $(BINS)
