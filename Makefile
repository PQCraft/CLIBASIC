BASE_CFLAGS = --std=c99 -Wall -Wextra -Ofast -lm -lreadline

ifndef OS

C = gcc
CFLAGS = $(BASE_CFLAGS)
ifeq ($(shell uname -s), Darwin)
ifeq ($(shell [ -d ~/.brew/opt/readline/include ] && echo true), true)
CFLAGS += -I~/.brew/opt/readline/include
endif
ifeq ($(shell [ -d /opt/homebrew/opt/readline/include ] && echo true), true)
CFLAGS += -I/opt/homebrew/opt/readline/include
endif
ifeq ($(shell [ -d /usr/local/opt/readline/include ] && echo true), true)
CFLAGS += -I/usr/local/opt/readline/include
endif
ifeq ($(shell [ -d ~/.brew/opt/readline/lib ] && echo true), true)
CFLAGS += -L~/.brew/opt/readline/lib
endif
ifeq ($(shell [ -d /opt/homebrew/opt/readline/lib ] && echo true), true)
CFLAGS += -L/opt/homebrew/opt/readline/lib
endif
ifeq ($(shell [ -d /usr/local/opt/readline/lib ] && echo true), true)
CFLAGS += -L/usr/local/opt/readline/lib
endif
else
ifeq ($(shell uname -o), Android)
CFLAGS += -s
else
CFLAGS += -s -no-pie
endif
endif

CBITS = $(shell getconf LONG_BIT)

BUILD_TO = clibasic
BUILD32 = $(C) clibasic.c -m32 $(CFLAGS) -DB32 -o $(BUILD_TO) && chmod +x $(BUILD_TO)
ifeq (,$(CBITS))
BUILD__ = $(BUILD32)
else
BUILD__ = $(C) clibasic.c $(CFLAGS) -DB$(CBITS) -o $(BUILD_TO) && chmod +x $(BUILD_TO)
endif

MAN_PATH = docs/clibasic.man

ifeq ($(shell id -u), 0)
MAN_INSTALL_PATH = /usr/share/man/man1/clibasic.1
INSTALL_TO = /usr/bin/clibasic
else
MAN_INSTALL_PATH = ~/.local/share/man/man1/clibasic.1
INSTALL_TO = ~/.local/bin/clibasic
endif
INSTALL = mkdir -p $(shell dirname -- $(INSTALL_TO)) $(shell dirname -- $(MAN_INSTALL_PATH)); cp $(BUILD_TO) $(INSTALL_TO); cp $(MAN_PATH) $(MAN_INSTALL_PATH); gzip -f $(MAN_INSTALL_PATH)

UNINSTALL = rm -f $(INSTALL_TO) $(MAN_INSTALL_PATH).gz

RUN = ./$(BUILD_TO)

CLEAN = rm -f clibasic

.ONESHELL:

.PHONY: all all32 build build32 update install install32 run clean cross

all: clean build run

all32: clean build32 run

build:
	$(BUILD__)

build32:
	$(BUILD32)

update:
	printf "\\e[0m\\e[31;1mAre you sure? [y/N]:\\e[0m "; read -n 1 I; [ ! "$$I" == "" ] && printf "\\n" &&\
([[ ! "$$I" =~ ^[^Yy]$$ ]] && sh -c 'git restore . && git pull' &> /dev/null && chmod +x *.sh) || exit 0

install:
	if [ ! -f $(BUILD_TO) ]; then $(BUILD__); fi
	$(INSTALL)

install32:
	if [ ! -f $(BUILD_TO) ]; then $(BUILD32); fi
	$(INSTALL)

uninstall:
	$(UNINSTALL)

run:
ifeq (32,$(CBITS))
	[ ! -f "$(BUILD_TO)" ] && ($(BUILD32))
else
	[ ! -f "$(BUILD_TO)" ] && ($(BUILD__))
endif
	$(RUN)

clean:
	$(CLEAN)

cross:
ifeq ($(MAKECMDGOALS), cross)
	@$(MAKE) cross all
else
	@$(eval C = x86_64-w64-mingw32-gcc)
	@$(eval C32 = i686-w64-mingw32-gcc)
	@$(eval CFLAGS = $(BASE_CFLAGS) -s -Ilib)
	@$(eval BUILD_TO = clibasic.exe)
	@$(eval INSTALL_TO = "$$HOME/.wine/drive_c/windows/system32/")
	@$(eval INSTALL = cp $(BUILD_TO) *.dll $(INSTALL_TO))
	@$(eval BUILD32 = cp -f lib/win32/*.dll . && $(C32) clibasic.c -m32 $(CFLAGS) -Llib/win32 -DB32 -o $(BUILD_TO) && chmod -x $(BUILD_TO))
ifeq (,$(CBITS))
	@$(eval BUILD__ = $(BUILD32))
else
	@$(eval BUILD__ = cp -f lib/win64/*.dll . && $(C) clibasic.c $(CFLAGS) -Llib/win64 -DB$(CBITS) -o $(BUILD_TO) && chmod -x $(BUILD_TO))
endif
	@$(eval RUN = wineconsole .\\$(BUILD_TO))
	@$(eval CLEAN = rm -f clibasic.exe *.dll)
endif
	@true

vt:
	@$(eval CFLAGS = $(CFLAGS) -DFORCE_VT)
	@$(eval BUILD32 = cp -f lib/win32/*.dll . && $(C32) clibasic.c -m32 $(CFLAGS) -Llib/win32 -DB32 -o $(BUILD_TO) && chmod -x $(BUILD_TO))
ifeq (,$(CBITS))
	@$(eval BUILD__ = $(BUILD32))
else
	@$(eval BUILD__ = cp -f lib/win64/*.dll . && $(C) clibasic.c $(CFLAGS) -Llib/win64 -DB$(CBITS) -o $(BUILD_TO) && chmod -x $(BUILD_TO))
endif
	@true

else

C = gcc

CFLAGS = $(BASE_CFLAGS) -Ilib -s -D_CRT_NONSTDC_NO_WARNINGS

BUILD_TO = clibasic.exe
BUILD64 = xcopy lib\win64\*.dll . /Y && $(C) clibasic.c -m64 $(CFLAGS) -Llib\win64 -DB64 -o $(BUILD_TO)
BUILD32 = xcopy lib\win32\*.dll . /Y && $(C) clibasic.c -m32 $(CFLAGS) -Llib\win32 -DB32 -o $(BUILD_TO)

INSTALL_TO = C:\windows\system32
INSTALL = xcopy *.dll $(INSTALL_TO) /Y && xcopy $(BUILD_TO) $(INSTALL_TO) /Y

UNINSTALL = del $(INSTALL_TO)\\$(BUILD_TO)

.PHONY: all all32 build build32 update run clean

all: clean build run

all32: clean build32 run

build:
	$(BUILD64)

build32:
	$(BUILD32)

update:
	git restore . & git pull

install:
	$(BUILD64)
	$(INSTALL)

install32:
	$(BUILD32)
	$(INSTALL)

run:
	.\\$(BUILD_TO)

clean:
	del /q /f $(BUILD_TO) *.dll

vt:
ifeq ($(MAKECMDGOALS), vt)
	@$(MAKE) vt all
else
	@$(eval CFLAGS = $(CFLAGS) -DFORCE_VT)
endif
	@echo > nul

endif

