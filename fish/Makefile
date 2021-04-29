CC=gcc
CFLAGS=-Wall -std=c99 -o2 -g
LDFLAGS=-g
TARGET=fish cmdline_test


all: $(TARGET)

fish: cmdline.o fish.o
	$(CC) $(LDFLAGS) $^ -o $@

cmdline_test: cmdline.o cmdline_test.o
	$(CC) $(LDFLAGS) $^ -o $@

cmdline.o: cmdline.c cmdline.h
	$(CC) $(CFLAGS) -c -o $@ $<

cmdline_test.o: cmdline_test.c 
	$(CC) $(CFLAGS) -c -o $@ $<

fish.o: fish.c 
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o

mrproper: clean
	rm -f $(TARGET)