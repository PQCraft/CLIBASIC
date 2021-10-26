/*  --------------------------------------------------------------------------------------------  */
/* |                               CLIBASIC header for extensions                               | */
/*  --------------------------------------------------------------------------------------------  */
// 
// Required:
// 
//   char cbext_name[]
//     Name of the extension (cannot be empty)
// 
//   bool cbext_init(cb_extargs)
//     Initialize pointers to internal CLIBASIC functions, set up the extension for use, and
//     return true if successful, otherwise, return false
// 
// 
// Optional:
// 
//   int cbext_runcmd(int, char**, uint8_t*, int32_t*)
//     Run a command utilizing the arguments (argument count, arguments (a call to solvearg([n])
//     is needed to solve an argument), argument types, argument lengths) and return 255 if no
//     match is found, an error code if an error is encountered, or 0 if execution succeeds
// 
//   cb_funcret cbext_runfunc(int, char**, uint8_t*, int32_t*, char*);
//     Run a function utilizing the arguments (argument count, arguments (will be pre-solved if
//     cbext_chkfuncsolve does not exist or returns false when passed the function name),
//     argument types, argument lengths, output buffer) and return {255, 0} if no match is
//     found, {error code, *} if an error is encountered, or {0, function type} if execution
//     succeeds
// 
//   int cbext_runlogic(char*, char**, int32_t, int32_t)
//     Run a logic command utilizing the arguments (raw input, split raw input ([0]) and empty
//     buffer for use in copying raw input into and performing actions on the copied raw input
//     ([1]), position of command in raw input, position of arguments in raw input) and return
//     255 if no match is found, an error code if an error is encountered, or 0 if execution is
//     successful
// 
//   bool cbext_chkfuncsolve(char*)
//     Return true if a function requires raw arguments, false otherwise
// 
//   void cbext_clearGlobals()
//     Reset internal variables usually before displaying the prompt
// 
//   void cbext_promptReady()
//     Prepare internal variables before displaying the prompt (guaranteed to only run before the
//     prompt is displayed)
// 
//   bool cbext_deinit()
//     Clean up extension before exiting
// 
// 
// Notes:
//   - It is a good idea to make all commands and functions adhere to '[Extension].[Cmd/Func]'
//     (unless overriding internal CLIBASIC commands or functions) to avoid naming conflicts.
//     This can be done with defining a macro as a string and passing 'MACRO".CMD/FUNC"' to
//     chkCmd as so:
//       #define P "TEST." // 'P' for 'Prefix'
//       ...
//       if (chkCmd(1, P"TEST")) {...

#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>

typedef struct {
    bool inuse;   // true if the spot is in use, false otherwise
    char* name;   // name of the variable
    uint8_t type; // type of the variable, 1 = string, 2 = number
    int32_t size; // max index of variable, -1 = normal variable, >= 0 = array, to get the size of the array, add 1
    char** data;  // array of strings containing the value(s)
} cb_var;

typedef struct {
    FILE* fptr;   // pointer to FILE* struct to read from and write to the file
    int32_t size; // file size at open
} cb_file;

typedef struct {
    int cerr;  // error to return
    int ftype; // type of the function, 1 = string, 2 = number
} cb_funcret;

typedef struct {
    bool bold;         // bold text attribute
    bool italic;       // italic text attribute
    bool underln;      // underline text attribute
    bool underlndbl;   // double underline text attribute
    bool underlnsqg;   // squiggly underline text attribute
    bool strike;       // strikethrough text attribute
    bool overln;       // overline text attribute
    bool dim;          // dim text attribute
    bool blink;        // blink text attribute
    bool hidden;       // hidden text attribute
    bool reverse;      // reverse colors text attribute
    int  underlncolor; // underline color (0-255) text attribute
    bool fgce;         // fgc enabled text attribute
    bool bgce;         // bgc enabled text attribute
    bool truecolor;    // truecolor enabled text attribute
    uint8_t fgc;       // 8-bit/standard fgc text attribute
    uint8_t bgc;       // 8-bit/standard bgc text attribute
    uint32_t truefgc;  // 24-bit/truecolor fgc text attribute
    uint32_t truebgc;  // 24-bit/truecolor bgc text attribute
} cb_txt;

typedef struct {
    char* VER;                                      // CLIBASIC version
    char* BVER;                                     // bits ("64"/"32"/"?")
    char* OSVER;                                    // os name ("Linux", "Windows", ...)
    int* cerr;                                      // pointer to the variable storing the error number
    int* retval;                                    // pointer to the variable read by the CLIBASIC function _RET()
    int* fileerror;                                 // pointer to the variable read by the CLIBASIC function _FILEERROR()
    int* varmaxct;                                  // pointer to the amount of variable spots allocated
    cb_var* vardata;                                // pointer to the variable spots
    int* filemaxct;                                 // pointer to the amount of file spots allocated
    cb_file* filedata;                              // pointer to the file spots
    char** chkCmdPtr;                               // pointer to a pointer of the char array to be compared by chkCmd()
    cb_txt* txtattrib;                              // pointer to the struct that stores text attributes
    int* curx;                                      // pointer to the text cursor x position
    int* cury;                                      // pointer to the text cursor y position
    void (*getCurPos)(void);                        // update the text cursor position
    char* (*gethome)(void);                         // get the user's home directory
    void (*seterrstr)(char*);                       // set the error string some errors require
    char* (*basefilename)(char*);                   // gets the file name off of a file path
    char* (*pathfilename)(char*);                   // gets the path name off of a file path
    int (*openFile)(char*, char*);                  // opens a file in a new file spot and returns the position
    bool (*closeFile)(int);                         // closes a file spot or all if supplied with -1 and returns true if successful, false otherwise
    bool (*cbrm)(char*);                            // removes a file or directory and returns true if successful, false otherwise
    uint64_t (*usTime)(void);                       // returns the time in nanoseconds
    uint64_t (*timer)(void);                        // returns the timer ticks in nanoseconds
    void (*resetTimer)(void);                       // resets the timer
    void (*cb_wait)(uint64_t);                      // waits for a certain amount of nanoseconds
    void (*updateTxtAttrib)(void);                  // reads the text attribute struct and applies the changes
    double (*randNum)(double, double);              // returns a random double from within a range
    bool (*chkCmd)(int, ...);                       // compares chkCmdPtr to strings supplied after the string count and returns true if a match is found, false otherwise
    bool (*isSpChar)(char);                         // checks if a char is a special character ('+', '-', '*', '\', & '^')
    bool (*isExSpChar)(char);                       // checks if a char is a special character but with more chars ('=', '>', '<', & ',')
    bool (*isValidVarChar)(char);                   // checks if a char is valid in a variable name
    bool (*isValidHexChar)(char);                   // checks if a char is valid in a hexadecimal number (0-F)
    int (*getArgCt)(char*);                         // returns the argument count of raw input
    int (*getArg)(int, char*, char*);               // writes the argument number in argument 1 of raw input from argument 2 into argument 3 and returns the length
    int (*getArgO)(int, char*, char*, int32_t);     // writes the argument number in argument 1 of raw input from argument 2 into argument 3 and returns the end, pass this to argument 4 on next call
    void (*getStr)(char*, char*);                   // returns a string after evaluating any backslash escape codes
    uint8_t (*getType)(char*);                      // gets the type of raw input ("\"test\"" returns 1 (string), "0" returns 2 (number), and "TEST" returns 255 (variable))
    uint8_t (*getVar)(char*, char*);                // puts the value of the variable specified by argument 1 in argument 2 and returns the type (1 = string, 2 = number)
    bool (*setVar)(char*, char*, uint8_t, int32_t); // sets the variable specified by argument 1 to the value specified by argument 2 and sets the type to argument 3 and size to argument 4
    bool (*delVar)(char*);                          // deletes a variable
    uint8_t (*getVal)(char*, char*);                // solves raw input in argument 1, writes the value into argument 2, and returns the type (1 = string, 2 = number, 255 = blank) or 0 on failure
    bool (*solvearg)(int);                          // solves an argument for commands as some commands may want to read from raw input
    uint8_t (*logictest)(char*);                    // takes raw input, tests it, and returns -1 on failure, 0 if false, and 1 if true
    void (*printError)(int);                        // prints a built-in error string
} cb_extargs;
