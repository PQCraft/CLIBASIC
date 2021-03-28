/* termiWin.h
*
* 	Copyright (C) 2017 Christian Visintin - christian.visintin1997@gmail.com
*
* 	This file is part of "termiWin: a termios porting for Windows"
*
*   termiWin is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   termiWin is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with termiWin.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef TERMIWIN_H_
#define TERMIWIN_H_

#define TERMIWIN_VERSION "1.2.0"
#define TERMIWIN_MAJOR_VERSION 1
#define TERMIWIN_MINOR_VERSION 2

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

/*Redefining functions from winsock to termiWin. This is very important since winsock2 defines functions such as close as closesocket we have to redefine it*/

#ifndef TERMIWIN_DONOTREDEFINE
//#define read read_serial
//#define write serial_write
#define open open_serial
#define close close_serial
#define select select_serial
#endif

// ssize_t
//#if SIZE_MAX == UINT_MAX
//typedef int ssize_t;        /* common 32 bit case */
//#define SSIZE_MIN  INT_MIN
//#define SSIZE_MAX  INT_MAX
//#elif SIZE_MAX == ULONG_MAX
//typedef long ssize_t;       /* linux 64 bits */
//#define SSIZE_MIN  LONG_MIN
//#define SSIZE_MAX  LONG_MAX
//#elif SIZE_MAX == ULLONG_MAX
//typedef long long ssize_t;  /* windows 64 bits */
//#define SSIZE_MIN  LLONG_MIN
//#define SSIZE_MAX  LLONG_MAX
//#endif

//Serial options - Linux -> Windows

/*setAttr flags - ~ in front of flags -> deny them*/

//iFlag

#define INPCK		0x00004000 	/*If this bit is set, input parity checking is enabled. If it is not set, no checking at all is done for parity errors on input; the characters are simply passed through to the application.*/
#define IGNPAR	0x00001000	/*If this bit is set, any byte with a framing or parity error is ignored. This is only useful if INPCK is also set.*/
#define PARMRK	0x00040000	/*If this bit is set, input bytes with parity or framing errors are marked when passed to the program. This bit is meaningful only when INPCK is set and IGNPAR is not set.*/
#define ISTRIP	0x00008000	/*If this bit is set, valid input bytes are stripped to seven bits; otherwise, all eight bits are available for programs to read. */
#define IGNBRK	0x00000400	/*If this bit is set, break conditions are ignored. */
#define BRKINT	0x00000100	/*If this bit is set and IGNBRK is not set, a break condition clears the terminal input and output queues and raises a SIGINT signal for the foreground process group associated with the terminal. */
#define IGNCR		0x00000800	/*If this bit is set, carriage return characters ('\r') are discarded on input. Discarding carriage return may be useful on terminals that send both carriage return and linefeed when you type the RET key. */
#define ICRNL		0x00000200	/*If this bit is set and IGNCR is not set, carriage return characters ('\r') received as input are passed to the application as newline characters ('\n').*/
#define INLCR		0x00002000	/*If this bit is set, newline characters ('\n') received as input are passed to the application as carriage return characters ('\r').*/
#define IXOFF		0x00010000	/*If this bit is set, start/stop control on input is enabled. In other words, the computer sends STOP and START characters as necessary to prevent input from coming in faster than programs are reading it. The idea is that the actual terminal hardware that is generating the input data responds to a STOP character by suspending transmission, and to a START character by resuming transmission.*/
#define IXON		0x00020000	/*If this bit is set, start/stop control on output is enabled. In other words, if the computer receives a STOP character, it suspends output until a START character is received. In this case, the STOP and START characters are never passed to the application program. If this bit is not set, then START and STOP can be read as ordinary characters.*/

//lFlag

#define ICANON	0x00001000	/*This bit, if set, enables canonical input processing mode. Otherwise, input is processed in noncanonical mode. */
#define ECHO		0x00000100	/*If this bit is set, echoing of input characters back to the terminal is enabled.*/
#define ECHOE		0x00000200	/*If this bit is set, echoing indicates erasure of input with the ERASE character by erasing the last character in the current line from the screen. Otherwise, the character erased is re-echoed to show what has happened (suitable for a printing terminal). */
#define ECHOK		0x00000400	/*This bit enables special display of the KILL character by moving to a new line after echoing the KILL character normally. The behavior of ECHOKE (below) is nicer to look at.*/
#define ECHONL	0x00000800	/*If this bit is set and the ICANON bit is also set, then the newline ('\n') character is echoed even if the ECHO bit is not set. */
#define ISIG		0x00004000	/*This bit controls whether the INTR, QUIT, and SUSP characters are recognized. The functions associated with these characters are performed if and only if this bit is set. Being in canonical or noncanonical input mode has no effect on the interpretation of these characters. */
#define IEXTEN	0x00002000	/*On BSD systems and GNU/Linux and GNU/Hurd systems, it enables the LNEXT and DISCARD characters.*/
#define NOFLSH	0x00008000	/*Normally, the INTR, QUIT, and SUSP characters cause input and output queues for the terminal to be cleared. If this bit is set, the queues are not cleared. */
#define TOSTOP	0x00010000	/*If this bit is set and the system supports job control, then SIGTTOU signals are generated by background processes that attempt to write to the terminal.*/

//cFlag

#define CSTOPB	0x00001000	/*If this bit is set, two stop bits are used. Otherwise, only one stop bit is used. */
#define PARENB	0x00004000	/*If this bit is set, generation and detection of a parity bit are enabled*/
#define PARODD	0x00008000	/*This bit is only useful if PARENB is set. If PARODD is set, odd parity is used, otherwise even parity is used. */
#define CSIZE		0x00000c00	/*This is a mask for the number of bits per character. */
#define CS5			0x00000000	/*This specifies five bits per byte. */
#define CS6			0x00000400	/*This specifies six bits per byte. */
#define CS7			0x00000800	/*This specifies seven bits per byte. */
#define CS8			0x00000c00	/*This specifies eight bits per byte. */
#define CLOCAL 	0x00000000 	/*Ignore modem control lines -> ignore data carrier detected - not implementable in windows*/
#define CREAD 	0x00000000 /*Enable receiver - if is not set no character will be received*/

//oFlag

#define OPOST	0x00000100 	/*If this bit is set, output data is processed in some unspecified way so that it is displayed appropriately on the terminal device. This typically includes mapping newline characters ('\n') onto carriage return and linefeed pairs. */

//cc

#define VEOF 0
#define VEOL 1
#define VERASE 2
#define VINTR 3
#define VKILL 4
#define VMIN 5 /*If set to 0, serial communication is NOT-BLOCKING, otherwise is BLOCKING*/
#define VQUIT 6
#define VSTART 7
#define VSTOP 8
#define VSUSP 9
#define VTIME 10

//END OF setAttr flags

/*Controls*/
#define TIOMBIC DTR_CONTROL_DISABLE
#define TIOMBIS DTR_CONTROL_ENABLE
#define CRTSCTS RTS_CONTROL_ENABLE

/*Others*/
#define NCCS 11

//Baud speed
#define B110 CBR_110
#define B300 CBR_300
#define B600 CBR_600
#define B1200 CBR_2400
#define B2400 CBR_2400
#define B4800 CBR_4800
#define B9600 CBR_9600
#define B19200 CBR_19200
#define B38400 CBR_38400
#define B57600 CBR_57600
#define B115200 CBR_115200

/*Attributes optional_actions*/
#define TCSANOW 0
#define TCSADRAIN 1
#define TCSAFLUSH 2

/*TCFLUSH options*/
#define TCIFLUSH 0
#define TCOFLUSH 1
#define TCIOFLUSH 2

/*TCFLOW optons*/

#define TCOOFF 0
#define TCOON 1
#define TCIOFF 2
#define TCION 3

//typdef
typedef unsigned tcflag_t; /*This is an unsigned integer type used to represent the various bit masks for terminal flags.*/
typedef unsigned cc_t; /*This is an unsigned integer type used to represent characters associated with various terminal control functions.*/
typedef unsigned speed_t; /*used for terminal baud rates*/

typedef struct termios
{

	tcflag_t c_iflag; /*input modes*/
	tcflag_t c_oflag; /*output modes*/
	tcflag_t c_cflag; /*control modes*/
	tcflag_t c_lflag; /*local modes*/
	cc_t c_cc[NCCS]; /*special character*/

} termios;

//Serial configuration functions

int tcgetattr(int fd, struct termios *termios_p);
int tcsetattr(int fd, int optional_actions, const struct termios *termios_p);
int tcsendbreak(int fd, int duration);
int tcdrain(int fd);
int tcflush(int fd, int queue_selector);
int tcflow(int fd, int action);
void cfmakeraw(struct termios *termios_p);
speed_t cfgetispeed(const struct termios *termios_p);
speed_t cfgetospeed(const struct termios *termios_p);
int cfsetispeed(struct termios *termios_p, speed_t speed);
int cfsetospeed(struct termios *termios_p, speed_t speed);
int cfsetspeed(struct termios * termios_p, speed_t speed);

//Write/Read/Open/Close/Select Functions

ssize_t read_serial(int fd, void* buffer, size_t count);
ssize_t write_serial(int fd, const void* buffer, size_t count);
int open_serial(const char* portname, int opt);
int close_serial(int fd);
int select_serial(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

//get Handle out of the COM structure
HANDLE getHandle();

#endif

#ifndef _WIN32
#pragma message("-Warning: termiWin requires a Windows system!")
#endif

/* termiWin.c
*
*   Copyright (C) 2017 Christian Visintin - christian.visintin1997@gmail.com
*
*   This file is part of "termiWin: a termios porting for Windows"
*
*   termiWin is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   termiWin is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with termiWin.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifdef _WIN32
#include <fcntl.h>
#include <stdlib.h>

typedef struct COM {
  HANDLE hComm;
  int fd; //Actually it's completely useless
  char port[128];
} COM;

DCB SerialParams = { 0 }; //Initializing DCB structure
struct COM com;
COMMTIMEOUTS timeouts = { 0 }; //Initializing COMMTIMEOUTS structure

//LOCAL functions

//nbyte 0->7

int getByte(tcflag_t flag, int nbyte, int nibble) {

  int byte;
  if (nibble == 1)
    byte = (flag >> (8 * (nbyte)) & 0x0f);
  else
    byte = (flag >> (8 * (nbyte)) & 0xf0);
  return byte;
}

//INPUT FUNCTIONS

int getIXOptions(tcflag_t flag) {

#define i_IXOFF 0x01
#define i_IXON 0x02
#define i_IXOFF_IXON 0x03
#define i_PARMRK 0x04
#define i_PARMRK_IXOFF 0x05
#define i_PARMRK_IXON 0x06
#define i_PARMRK_IXON_IXOFF 0x07

  int byte = getByte(flag, 1, 1);

  return byte;
}

//LOCALOPT FUNCTIONS

int getEchoOptions(tcflag_t flag) {

#define l_NOECHO 0x00
#define l_ECHO 0x01
#define l_ECHO_ECHOE 0x03
#define l_ECHO_ECHOK 0x05
#define l_ECHO_ECHONL 0x09
#define l_ECHO_ECHOE_ECHOK 0x07
#define l_ECHO_ECHOE_ECHONL 0x0b
#define l_ECHO_ECHOE_ECHOK_ECHONL 0x0f
#define l_ECHO_ECHOK_ECHONL 0x0d
#define l_ECHOE 0x02
#define l_ECHOE_ECHOK 0x06
#define l_ECHOE_ECHONL 0x0a
#define l_ECHOE_ECHOK_ECHONL 0x0e
#define l_ECHOK 0x04
#define l_ECHOK_ECHONL 0x0c
#define l_ECHONL 0x08

  int byte = getByte(flag, 1, 1);
  return byte;
}

int getLocalOptions(tcflag_t flag) {

#define l_ICANON 0x10
#define l_ICANON_ISIG 0x50
#define l_ICANON_IEXTEN 0x30
#define l_ICANON_NOFLSH 0x90
#define l_ICANON_ISIG_IEXTEN 0x70
#define l_ICANON_ISIG_NOFLSH 0xd0
#define l_ICANON_IEXTEN_NOFLSH 0xb0
#define l_ICANON_ISIG_IEXTEN_NOFLSH 0xf0
#define l_ISIG 0x40
#define l_ISIG_IEXTEN 0x60
#define l_ISIG_NOFLSH 0xc0
#define l_ISIG_IEXTEN_NOFLSH 0xe0
#define l_IEXTEN 0x20
#define l_IEXTEN_NOFLSH 0xa0
#define l_NOFLSH 0x80

  int byte = getByte(flag, 1, 0);
  return byte;
}

int getToStop(tcflag_t flag) {

#define l_TOSTOP 0x01

  int byte = getByte(flag, 1, 1);
  return byte;
}

//CONTROLOPT FUNCTIONS

int getCharSet(tcflag_t flag) {

  //FLAG IS MADE UP OF 8 BYTES, A FLAG IS MADE UP OF A NIBBLE -> 4 BITS, WE NEED TO EXTRACT THE SECOND NIBBLE (1st) FROM THE FIFTH BYTE (6th).
  int byte = getByte(flag, 1, 1);

  switch (byte) {

  case 0X0:
    return CS5;
    break;

  case 0X4:
    return CS6;
    break;

  case 0X8:
    return CS7;
    break;

  case 0Xc:
    return CS8;
    break;

  default:
    return CS8;
    break;
  }
}

int getControlOptions(tcflag_t flag) {

#define c_ALL_ENABLED 0xd0
#define c_PAREVEN_CSTOPB 0x50
#define c_PAREVEN_NOCSTOPB 0x40
#define c_PARODD_NOCSTOPB 0xc0
#define c_NOPARENB_CSTOPB 0x10
#define c_ALL_DISABLED 0x00

  int byte = getByte(flag, 1, 0);
  return byte;
}

//LIBFUNCTIONS

int tcgetattr(int fd, struct termios* termios_p) {

  if (fd != com.fd) return -1;
  int ret = 0;

  ret = GetCommState(com.hComm, &SerialParams);

  return 0;
}

int tcsetattr(int fd, int optional_actions, const struct termios* termios_p) {

  if (fd != com.fd) return -1;
  int ret = 0;

  //Store flags into local variables
  tcflag_t iflag = termios_p->c_iflag;
  tcflag_t lflag = termios_p->c_lflag;
  tcflag_t cflag = termios_p->c_cflag;
  tcflag_t oflag = termios_p->c_oflag;

  //iflag

  int IX = getIXOptions(iflag);

  if ((IX == i_IXOFF_IXON) || (IX == i_PARMRK_IXON_IXOFF)) {

    SerialParams.fOutX = TRUE;
    SerialParams.fInX = TRUE;
    SerialParams.fTXContinueOnXoff = TRUE;
  }

  //lflag
  int EchoOpt = getEchoOptions(lflag);
  int l_opt = getLocalOptions(lflag);
  int tostop = getToStop(lflag);

  //Missing parameters...

  //cflags

  int CharSet = getCharSet(cflag);
  int c_opt = getControlOptions(cflag);

  switch (CharSet) {

  case CS5:
    SerialParams.ByteSize = 5;
    break;

  case CS6:
    SerialParams.ByteSize = 6;
    break;

  case CS7:
    SerialParams.ByteSize = 7;
    break;

  case CS8:
    SerialParams.ByteSize = 8;
    break;
  }

  switch (c_opt) {

  case c_ALL_ENABLED:
    SerialParams.Parity = ODDPARITY;
    SerialParams.StopBits = TWOSTOPBITS;
    break;

  case c_ALL_DISABLED:
    SerialParams.Parity = NOPARITY;
    SerialParams.StopBits = ONESTOPBIT;
    break;

  case c_PAREVEN_CSTOPB:
    SerialParams.Parity = EVENPARITY;
    SerialParams.StopBits = TWOSTOPBITS;
    break;

  case c_PAREVEN_NOCSTOPB:
    SerialParams.Parity = EVENPARITY;
    SerialParams.StopBits = ONESTOPBIT;
    break;

  case c_PARODD_NOCSTOPB:
    SerialParams.Parity = ODDPARITY;
    SerialParams.StopBits = ONESTOPBIT;
    break;

  case c_NOPARENB_CSTOPB:
    SerialParams.Parity = NOPARITY;
    SerialParams.StopBits = TWOSTOPBITS;
    break;
  }

  //aflags

  /*
  int OP;
  if(oflag == OPOST)
  else ...
  */
  //Missing parameters...

  //special characters

  if (termios_p->c_cc[VEOF] != 0) SerialParams.EofChar = (char)termios_p->c_cc[VEOF];
  if (termios_p->c_cc[VINTR] != 0) SerialParams.EvtChar = (char)termios_p->c_cc[VINTR];

  if (termios_p->c_cc[VMIN] == 1) { //Blocking

    timeouts.ReadIntervalTimeout = 0;         // in milliseconds
    timeouts.ReadTotalTimeoutConstant = 0;    // in milliseconds
    timeouts.ReadTotalTimeoutMultiplier = 0;  // in milliseconds
    timeouts.WriteTotalTimeoutConstant = 0;   // in milliseconds
    timeouts.WriteTotalTimeoutMultiplier = 0; // in milliseconds

  } else { //Non blocking

    timeouts.ReadIntervalTimeout = termios_p->c_cc[VTIME] * 100;         // in milliseconds
    timeouts.ReadTotalTimeoutConstant = termios_p->c_cc[VTIME] * 100;    // in milliseconds
    timeouts.ReadTotalTimeoutMultiplier = termios_p->c_cc[VTIME] * 100;  // in milliseconds
    timeouts.WriteTotalTimeoutConstant = termios_p->c_cc[VTIME] * 100;   // in milliseconds
    timeouts.WriteTotalTimeoutMultiplier = termios_p->c_cc[VTIME] * 100; // in milliseconds
  }

  SetCommTimeouts(com.hComm, &timeouts);

  //EOF

  ret = SetCommState(com.hComm, &SerialParams);
  if (ret != 0)
    return 0;
  else
    return -1;
}

int tcsendbreak(int fd, int duration) {

  if (fd != com.fd) return -1;

  int ret = 0;
  ret = TransmitCommChar(com.hComm, '\x00');
  if (ret != 0)
    return 0;
  else
    return -1;
}

int tcdrain(int fd) {

  if (fd != com.fd) return -1;
  return FlushFileBuffers(com.hComm);
}

int tcflush(int fd, int queue_selector) {

  if (fd != com.fd) return -1;
  int rc = 0;

  switch (queue_selector) {

  case TCIFLUSH:
    rc = PurgeComm(com.hComm, PURGE_RXCLEAR);
    break;

  case TCOFLUSH:
    rc = PurgeComm(com.hComm, PURGE_TXCLEAR);
    break;

  case TCIOFLUSH:
    rc = PurgeComm(com.hComm, PURGE_RXCLEAR);
    rc *= PurgeComm(com.hComm, PURGE_TXCLEAR);
    break;

  default:
    rc = 0;
    break;
  }

  if (rc != 0)
    return 0;
  else
    return -1;
}

int tcflow(int fd, int action) {

  if (fd != com.fd) return -1;
  int rc = 0;

  switch (action) {

  case TCOOFF:
    rc = PurgeComm(com.hComm, PURGE_TXABORT);
    break;

  case TCOON:
    rc = ClearCommBreak(com.hComm);
    break;

  case TCIOFF:
    rc = PurgeComm(com.hComm, PURGE_RXABORT);
    break;

  case TCION:
    rc = ClearCommBreak(com.hComm);
    break;

  default:
    rc = 0;
    break;
  }

  if (rc != 0)
    return 0;
  else
    return -1;
}

void cfmakeraw(struct termios* termios_p) {

  SerialParams.ByteSize = 8;
  SerialParams.StopBits = ONESTOPBIT;
  SerialParams.Parity = NOPARITY;
}

speed_t cfgetispeed(const struct termios* termios_p) {

  return SerialParams.BaudRate;
}

speed_t cfgetospeed(const struct termios* termios_p) {

  return SerialParams.BaudRate;
}

int cfsetispeed(struct termios* termios_p, speed_t speed) {

  SerialParams.BaudRate = speed;
  return 0;
}

int cfsetospeed(struct termios* termios_p, speed_t speed) {

  SerialParams.BaudRate = speed;
  return 0;
}

int cfsetspeed(struct termios* termios_p, speed_t speed) {

  SerialParams.BaudRate = speed;
  return 0;
}

ssize_t read_serial(int fd, void* buffer, size_t count) {

  if (fd != com.fd) return -1;
  int rc = 0;
  int ret;

  ret = ReadFile(com.hComm, buffer, count, (long unsigned int *)&rc, NULL);

  if (ret == 0)
    return -1;
  else
    return rc;
}

ssize_t write_serial(int fd, const void* buffer, size_t count) {

  if (fd != com.fd) return -1;
  int rc = 0;
  int ret;

  ret = WriteFile(com.hComm, buffer, count, (long unsigned int *)&rc, NULL);

  if (ret == 0)
    return -1;
  else
    return rc;
}

int open_serial(const char* portname, int opt) {

  if (strlen(portname) < 4) return -1;

  // Set to zero
  memset(com.port, 0x00, 128);

  //COMxx
  size_t portSize = 0;
  if (strlen(portname) > 4) {
    portSize = sizeof(char) * strlen("\\\\.\\COM10") + 1;
    strncat_s(com.port, portSize, "\\\\.\\", strlen("\\\\.\\"));
  }
  //COMx
  else {
    portSize = sizeof(char) * 5;
  }

#ifdef _MSC_VER
  strncat_s(com.port, portSize, portname, 4);
#else
  strncat(com.port, portname, 4);
#endif
  com.port[portSize] = 0x00;

  switch (opt) {

  case O_RDWR:
    com.hComm = CreateFile(com.port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    break;

  case O_RDONLY:
    com.hComm = CreateFile(com.port, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    break;

  case O_WRONLY:
    com.hComm = CreateFile(com.port, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    break;
  }

  if (com.hComm == INVALID_HANDLE_VALUE) {
    return -1;
  }
  com.fd = atoi(portname + 3); // COMx and COMxx
  SerialParams.DCBlength = sizeof(SerialParams);
  return com.fd;
}

int close_serial(int fd) {

  int ret = CloseHandle(com.hComm);
  if (ret != 0)
    return 0;
  else
    return -1;
}

int select_serial(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout) {

  SetCommMask(com.hComm, EV_RXCHAR);
  DWORD dwEventMask;
  if (WaitCommEvent(com.hComm, &dwEventMask, NULL) == 0) {
    return -1; // Return -1 if failed
  }
  if (dwEventMask == EV_RXCHAR) {
    return com.fd;
  } else {
    if (readfds) {
      // Clear file descriptor if event is not RXCHAR
      FD_CLR(com.fd, readfds);
    }
  }
  // NOTE: write event not detectable!
  // NOTE: no timeout
  return 0; // No data
}

//Returns hComm from the COM structure
HANDLE getHandle() {
  return com.hComm;
}

#endif

#endif