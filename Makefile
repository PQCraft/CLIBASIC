C = gcc

CFLAGS1 = -Wall -Wextra -O2
CFLAGS2 = -lm -ledit

all:
	rm -f ./clibasic
	$(C) $(CFLAGS1) clibasic.c -o clibasic $(CFLAGS2)
	chmod +x ./clibasic
	./clibasic

all32:
	rm -f ./clibasic
	$(C) $(CFLAGS1) -m32 clibasic.c -o clibasic $(CFLAGS2)
	chmod +x ./clibasic
	./clibasic

build:
	$(C) $(CFLAGS1) clibasic.c -o clibasic $(CFLAGS2)
	chmod +x ./clibasic

build32:
	$(C) $(CFLAGS1) -m32 clibasic.c -o clibasic $(CFLAGS2)
	chmod +x ./clibasic

run:
	./clibasic

debug:
	./clibasic --debug

cls:
	clear

clean:
	rm -f clibasic
