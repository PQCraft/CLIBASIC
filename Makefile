C = gcc

CFLAGS = -O2

all:
	rm -f ./clibasic
	$(C) $(CFLAGS) clibasic.c -o clibasic -lm -lreadline
	chmod +x ./clibasic
	./clibasic

all32:
	rm -f ./clibasic
	$(C) $(CFLAGS) -m32 clibasic.c -o clibasic -lm -lreadline
	chmod +x ./clibasic
	./clibasic

build:
	$(C) $(CFLAGS) clibasic.c -o clibasic -lm -lreadline
	chmod +x ./clibasic

build32:
	$(C) $(CFLAGS) -m32 clibasic.c -o clibasic -lm -lreadline
	chmod +x ./clibasic

run:
	./clibasic

debug:
	./clibasic --debug

cls:
	clear

clean:
	rm -f clibasic
