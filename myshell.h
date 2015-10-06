
/* These are some constants based on the size programs the system will execute */

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24

/* These are the required INCLUDE files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DELIMITER 1
#define VARIABLE 2
#define NUMBER 3
#define SETCOMMAND 4
#define STRING 5
#define QUOTE 6
#define EOL 9
#define FINISHED 10

#define MAX_LINE 80           /* The maximum length of a command */
#define MAX_PROMPT 40         /* The maximum length of the prompt */
#define MAX_TOKENS 20         /* maximum tokens per line */
#define HISTORY_BUFFER 1000   /* The number of commands to keep in history */
#define MAX_ALIASES 1000      /* maximum number of aliases */
#define MAX_PATH 250          /* maximum length for the path */
#define HELP_LINES 100        /* Number of Help lines */
#define BOOLEAN int
#define TRUE 1
#define FALSE 0
#define INVALID -1 
#define OK 0
#define NO_INPUT 1
#define TOO_LONG 2

typedef struct {
   char cmdLine[MAX_LINE];
   int cmdNumber;
} COMMAND;

typedef struct {
   char token[MAX_LINE];
   int tokenType;
} TOKENLIST;
TOKENLIST tokens[MAX_TOKENS];

COMMAND historyBuffer[HISTORY_BUFFER]; /* This is the cmd history */
COMMAND startupBuffer[HISTORY_BUFFER]; /* Used for startup config */

typedef struct {
   char helpLine[MAX_LINE];
}  HELPLINE;

typedef struct {
   char alias[MAX_LINE];
   char actualCmd[MAX_LINE];
} ALIASTYPE;

typedef struct {
   float version;
   BOOLEAN verbose;
   BOOLEAN script;
   int currentBuffer;
   char scriptFile[MAX_LINE];
   FILE *scriptFilePtr;
   char startupFile[MAX_LINE];
   int numStartCmds;              /* Number of lines in startup config */
   char prompt[MAX_PROMPT];
   int numAliases;
   ALIASTYPE aliases[MAX_ALIASES];
   HELPLINE helpLines[HELP_LINES];
   int numHelpLines;
   char path[MAX_PATH];
} CONFIGDB;

CONFIGDB configDB;

typedef enum { 
   HISTORY, 
   PROMPT, 
   BANG, 
   BANGBANG, 
   HELP, 
   EXIT, 
   ALIAS,
   CLS,
   CLEAR,
   ECHO,   
   NEWLINE, 
   VERBOSEON, 
   VERBOSEOFF, 
   STARTSCRIPT, 
   ENDSCRIPT,
   DUMPDB,
   CMDTOFORK,
   SETPATH,
   END
} Shellcommands;

Shellcommands symbol;
/*
#define HISTORY 1
#define BANG    2
#define BANGBANG 3
#define HELP 4
#define CLS 5
#define CLEAR 5
#define ECHO 6
#define NEWLINE 7
#define VERBOSEON 8
#define VERBOSEOFF 9
#define STARTSCRIPT 10
#define ENDSCRIPT 11
#define PROMPT 12
#define DUMPDB 13
#define CMDTOFORK 14
#define END 99
#define EXIT 99
*/
/**  Some global variables, these should eventually be collapsed into pass by reference */
 
int error(int err);
int warning(int warning);
static int getLine(char *prmpt, char *buff, size_t sz);
int debug(char *error);
void execute_dumpdb();
int parse_command(char *buffer, int iMode);
void init_helpDB();
void print_helpDB();
int execute_help();
int execute_cls();
int execute_prompt();
int execute_bang();
int execute_bangbang();
int execute_exit();
int execute_history();
BOOLEAN file_exists(const char * filename);
void myprintf(char *string);
static int getLine(char *prmpt, char *buff, size_t sz);
int load_config();
void init_script();
int run_command(char *cmdBuffer, int iMode);
void init_configDB();
int tokenize(char *cmdBuffer);
/*int get_next_token(char *buffer, int *tok)*/
