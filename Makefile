C = gcc

CFLAGS = -Wall -Wextra -O2 -ffreestanding -lm -lreadline --std=c99

CBITS = $(shell getconf LONG_BIT)

all:
	rm -f ./clibasic
	$(C) clibasic.c $(CFLAGS) -D B$(CBITS) -o clibasic
	chmod +x ./clibasic
	./clibasic

all32:
	rm -f ./clibasic
	$(C) clibasic.c -m32 $(CFLAGS) -D B32 -o clibasic
	chmod +x ./clibasic
	./clibasic

build:
	$(C) clibasic.c $(CFLAGS) -D B$(CBITS) -o clibasic
	chmod +x ./clibasic

build32:
	$(C) clibasic.c -m32 $(CFLAGS) -D B32 -o clibasic
	chmod +x ./clibasic

update:
	curl https://raw.githubusercontent.com/PQCraft/clibasic/master/clibasic.c > .tmp
	rm -f clibasic.c
	mv .tmp clibasic.c
	curl https://raw.githubusercontent.com/PQCraft/clibasic/master/commands.c > .tmp
	rm -f commands.c
	mv .tmp commands.c
	curl https://raw.githubusercontent.com/PQCraft/clibasic/master/functions.c > .tmp
	rm -f functions.c
	mv .tmp functions.c
	curl https://raw.githubusercontent.com/PQCraft/clibasic/master/logic.c > .tmp
	rm -f logic.c
	mv .tmp logic.c
	curl https://raw.githubusercontent.com/PQCraft/clibasic/master/Makefile > .tmp
	rm -f Makefile
	mv .tmp Makefile

install:
	if [ ! -f ./clibasic ]; then $(C) clibasic.c $(CFLAGS) -D B$(CBITS) -o clibasic; chmod +x ./clibasic; fi
	sudo rm -f /usr/bin/clibasic; sudo cp ./clibasic /usr/bin/clibasic

install32:
	if [ ! -f ./clibasic ]; then $(C) clibasic.c -m32 $(CFLAGS) -D B32 -o clibasic; chmod +x ./clibasic; fi
	sudo rm -f /usr/bin/clibasic; sudo cp ./clibasic /usr/bin/clibasic

run:
	./clibasic

debug:
	./clibasic --debug

clean:
	rm -f clibasic
