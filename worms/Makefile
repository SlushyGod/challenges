CC=gcc
CFLAGS=-w -O2
FILE=chal.c
NONX=-z execstack
NOCANARY=-fno-stack-protector
NOPIE=-no-pie

phony: all chal1 chal2 chal3 chal4

all: chal1 chal2 chal3 chal4

chal1:
	gcc $(CFLAGS) $(NOCANARY) $(NONX) $(NOPIE) -o chal1 $(FILE)

chal2:
	gcc $(CFLAGS) $(NONX) $(NOPIE) -o chal2 $(FILE)

chal3:
	gcc $(CFLAGS) $(NOPIE) -o chal3 $(FILE)

chal4:
	gcc $(CFLAGS) -o chal4 $(FILE)

clean:
	rm -f chal1 chal2 chal3 chal4
