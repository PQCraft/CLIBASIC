# CLIBASIC
<br>**C**ommand **L**ine **I**nterface **B**eginners **A**ll-purpose **S**ymbolic **I**nstruction **C**ode <br>
<br>
To build, use `make clean build`. <br>
To build and run, use `make clean build run`. <br>
To build and run while displaying debug info, use `make clean build debug`. <br>
<br>
**Requred packages:** <br>
Debian/Ubuntu: `libedit-dev` `libeditline-dev` <br>
Arch: `libedit` `editline` <br>
Apline: `libedit-dev` `editline-dev` <br>
<br>
**NOTES:** <br>
The prompt prints \r before printing out the text as an editline bug workaround, <br>
when using `PRINT`, remember to add `"\n"` (eg.: `PRINT "text", "\n"`, `PRINT "text" + "\n"`). 
