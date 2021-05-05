C = gcc

CFLAGS = -Wall -Wextra -O2 -s -no-pie -lm -lreadline --std=c99

CBITS = $(shell getconf LONG_BIT)

BUILD__ = $(C) clibasic.c $(CFLAGS) -DB$(CBITS) -o clibasic && chmod +x ./clibasic
BUILD32 = $(C) clibasic.c -m32 $(CFLAGS) -DB32 -o clibasic && chmod +x ./clibasic

INSTALL_TO = "/usr/bin/clibasic"

INSTALL = sudo rm -f $(INSTALL_TO); sudo cp ./clibasic $(INSTALL_TO)

all: clean build run

all32: clean build32 run

build:
	$(BUILD__)

build32:
	$(BUILD32)

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
	if [ ! -f ./clibasic ]; then $(BUILD__); fi
	$(INSTALL)

install32:
	if [ ! -f ./clibasic ]; then $(BUILD32); fi
	$(INSTALL)

run:
	./clibasic

debug:
	./clibasic --debug

clean:
	rm -f ./clibasic
