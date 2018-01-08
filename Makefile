CC = gcc
CFLAGS = -O3
vm: source/vm.c
	$(CC) $(CFLAGS) $? -o out/$@