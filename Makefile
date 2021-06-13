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
	printf "\\e[0m\\e[31;1mAre you sure? [y/N]: "; read -n 1 I; [ ! "$$I" == "" ] && printf "\\e[0m\\n" &&\
(([[ ! "$$I" =~ ^[^Yy]$$ ]]) && rm -rf ./[!.]* ./.[!.]* && git clone https://github.com/PQCraft/clibasic . && chmod +x *.sh) || exit 0

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
