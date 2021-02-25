C = gcc

CFLAGS = -O2

build:
	$(C) $(CFLAGS) clibasic.c -o clibasic
	chmod +x ./clibasic

build32:
	$(C) $(CFLAGS) -m32 clibasic.c -o clibasic
	chmod +x ./clibasic

run:
	./clibasic

debug:
	./clibasic --debug

cls:
	clear

clean:
	rm -f clibasic
