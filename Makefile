C = gcc

CFLAGS1 = -Wall -Wextra -O2
CFLAGS2 = -lm -ledit

CBITS = $(shell getconf LONG_BIT)

all:
	rm -f ./clibasic
	$(C) $(CFLAGS1) -D B$(CBITS) clibasic.c -o clibasic $(CFLAGS2)
	chmod +x ./clibasic
	./clibasic

all32:
	rm -f ./clibasic
	$(C) $(CFLAGS1) -m32 -D B32 clibasic.c -o clibasic $(CFLAGS2)
	chmod +x ./clibasic
	./clibasic

build:
	$(C) $(CFLAGS1) -D B$(CBITS) clibasic.c -o clibasic $(CFLAGS2)
	chmod +x ./clibasic

build32:
	$(C) $(CFLAGS1) -m32 -D B32 clibasic.c -o clibasic $(CFLAGS2)
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
	curl https://raw.githubusercontent.com/PQCraft/clibasic/master/Makefile > .tmp
	rm -f Makefile
	mv .tmp Makefile

install:
	if [ ! -f ./clibasic ]; then $(C) $(CFLAGS1) -D B$(CBITS) clibasic.c -o clibasic $(CFLAGS2); chmod +x ./clibasic; fi
	sudo rm -f /usr/bin/clibasic; sudo cp ./clibasic /usr/bin/clibasic

install32:
	if [ ! -f ./clibasic ]; then $(C) $(CFLAGS1) -m32 -D B32 clibasic.c -o clibasic $(CFLAGS2); chmod +x ./clibasic; fi
	sudo rm -f /usr/bin/clibasic; sudo cp ./clibasic /usr/bin/clibasic

run:
	./clibasic

debug:
	./clibasic --debug

clean:
	rm -f clibasic
