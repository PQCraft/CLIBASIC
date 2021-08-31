BASE_CFLAGS = --std=c99 -Wall -Wextra -Ofast -lm -lreadline

ifdef OS
ifeq ($(findstring :/, $(PATH)), :/)
IS_LINUX = true
else
IS_LINUX = false
endif
else
IS_LINUX = true
endif

ifeq ($(IS_LINUX), true)

C = gcc
CFLAGS = $(BASE_CFLAGS)
ifeq ($(shell uname -s), Darwin)
CFLAGS += -I/usr/local/opt/readline/include -L/usr/local/opt/readline/lib
else
ifeq ($(shell uname -o), Android)
CFLAGS += -s
else
CFLAGS += -s -no-pie
endif
endif

CBITS = $(shell getconf LONG_BIT)

BUILD_TO = "clibasic"
BUILD__ = $(C) clibasic.c $(CFLAGS) -DB$(CBITS) -o $(BUILD_TO) && chmod +x $(BUILD_TO)
BUILD32 = $(C) clibasic.c -m32 $(CFLAGS) -DB32 -o $(BUILD_TO) && chmod +x $(BUILD_TO)

INSTALL_TO = "/usr/bin/clibasic"
INSTALL = if [ "$$(id -u)" -eq 0 ]; then cp $(BUILD_TO) $(INSTALL_TO); else echo "Root privileges are needed to install."; fi

RUN = ./clibasic

CLEAN = rm -f clibasic

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
	if [ ! -f $(INSTALL_TO) ]; then $(BUILD__); fi
	$(INSTALL)

install32:
	if [ ! -f $(INSTALL_TO) ]; then $(BUILD32); fi
	$(INSTALL)

run:
	$(RUN)

clean:
	$(CLEAN)

.ONESHELL:

cross:
ifeq ($(MAKECMDGOALS), cross)
	@$(MAKE) cross all
else
	@$(eval C = x86_64-w64-mingw32-gcc)
	@$(eval C32 = i686-w64-mingw32-gcc)
	@$(eval CFLAGS = $(BASE_CFLAGS) -Ilib)
	@$(eval BUILD_TO = "clibasic.exe")
	@$(eval BUILD__ = cp -f lib/win64/*.dll . && $(C) clibasic.c $(CFLAGS) -Llib/win64 -DB$(CBITS) -o $(BUILD_TO) && chmod -x ./clibasic.exe)
	@$(eval BUILD32 = cp -f lib/win32/*.dll . && $(C32) clibasic.c -m32 $(CFLAGS) -Llib/win32 -DB32 -o $(BUILD_TO) && chmod -x ./clibasic.exe)
	@$(eval INSTALL_TO = "$$HOME/.wine/drive_c/windows/system32")
	@$(eval INSTALL = cp $(BUILD_TO) *.dll $(INSTALL_TO))
	@$(eval RUN = wineconsole clibasic.exe)
	@$(eval CLEAN = rm -f clibasic.exe *.dll)
endif
	@true

else

C = gcc

CFLAGS = $(BASE_CFLAGS) -Ilib -s -D_CRT_NONSTDC_NO_WARNINGS

BUILD_TO = clibasic.exe
BUILD64 = xcopy lib\win64\*.dll . /Y && $(C) clibasic.c -m64 $(CFLAGS) -Llib\win64 -DB64 -o $(BUILD_TO)
BUILD32 = xcopy lib\win32\*.dll . /Y && $(C) clibasic.c -m32 $(CFLAGS) -Llib\win32 -DB32 -o $(BUILD_TO)

INSTALL_TO = "C:\\windows\\system32"
INSTALL = xcopy *.dll $(INSTALL_TO) /Y && xcopy $(BUILD_TO) $(INSTALL_TO) /Y

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

endif

