CC = gcc
CFLAGS = -O3
vm.out: source/vm.c
	$(CC) $(CFLAGS) $? -o out/$@

clear:
	rm -rf out/* in/*

