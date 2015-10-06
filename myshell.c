/************************************************************


  Programmer - David Whipple
  
  Purpose    - This was originally written for an Operating
               Systems course @ Drexel, CS543.
               The project was to develop a shell for Unix, 
               using the stub source code in Operating Systems
               Concepts by Silberschatz, Galvin, & Gagne, 9th Edition.

  Change Log
--------------------------------------------------------------
  02/16/2014   Started, got stub code working..

**************************************************************/

#include "myshell.h"
/* ----------------------------------------------------------------- */
/* FUNCTION  parse:                                                  */
/*    This function takes an input line and parse it into tokens.    */
/* It first replaces all white spaces with zeros until it hits a     */
/* non-white space character which indicates the beginning of an     */
/* argument.  It saves the address to argv[], and then skips all     */
/* non-white spaces which constitute the argument.                   */
/* ----------------------------------------------------------------- */

void  parse(char *line, char **argv,int iMode)
{
     while (*line != '\0') {       /* if not the end of line ....... */ 
          while (*line == ' ' || *line == '\t' || *line == '\n' || *line == '"')
               *line++ = '\0';     /* replace white spaces with 0    */
          *argv++ = line;          /* save the argument position     */
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n' && *line != '"') 
               line++;             /* skip the argument until ...    */
     }
     *argv = '\0';                 /* mark the end of argument list  */
	
}

int fork_child_process(char **argv)
{
   pid_t pid;
   
   /* printf("Forking Command %s.\n",cmdString); */
   pid = fork();
   if (pid == -1) {
      /* -1 means an error occurred */
	  error(7);
    }
	else if (pid == 0) {
	   /* 0 means in child process */
	   execvp(*argv, argv);
	   /* printf("Hello from the child process!\n");*/
	   
	   /* execlp("ls",args); */
	   _exit(EXIT_SUCCESS); /* exit() can be unreliable, so use _exit() */
    }
	else {
	   /* If fork() returns a positive number, we are in the parent process 
	      and the return value is the PID of the newly created child */
	   int status;
	   /* printf("Child PID=%d.\n",pid); */
	   (void)waitpid(pid, &status, 0);
	}
	
	/*
	char *cmd[] = { "ls", "-l", (char *)0 };
    char *env[] = { "HOME=/usr/home", "LOGNAME=home", (char *)0 };
    ...
    ret = execve ("/bin/ls", cmd, env);

   */
}

int main(void) 
{
   int tokenNumber=0;
   char *args[MAX_LINE/2 + 1];            /* command line arguments */
   char cmdBuffer[MAX_LINE];              /* This is the current command */
   
   
   int isShellCmd = 0;                    /* Boolean for if is shell cmd */
   int returnSymbol;                      /* Used for parsing function return */
   int temp;                              /* Used to store function return value that isn't needed */
   char *token=NULL; 
   int iMode=TRUE;                        /* True if in interactive mode, false if running cmd */
   int shouldRun=1;                       /* flag to determine when to exit the program */
   int i;                                 /* Used for a loop variable */
   char *argv[64];
   
   
   init_configDB();                       /* Initialize the configuration database */
   
   load_config();                         /* loads the .startup configuration file */
  	
   /* Let's run those .startup commands */
   iMode=FALSE;
   for (i=0;i<configDB.numStartCmds;i++) {
      /* if (configDB.verbose) printf("Executing \"%s\".\n",startupBuffer[i].cmdLine);* /
      returnSymbol=run_command(startupBuffer[i].cmdLine, iMode);	  
   }
   iMode=TRUE;
   
   welcome_screen();                      /* prints minimum startup text */
   
   /* This is for the interactive shell */
   
   while ((shouldRun) && (iMode)) {
      /* if (configDB.verbose) printf("(%d)",configDB.currentBuffer); */

      temp = getLine(configDB.prompt, cmdBuffer, sizeof(cmdBuffer));
      /* save the entire command in the historyBuffer */
      strcpy(historyBuffer[configDB.currentBuffer].cmdLine, cmdBuffer);
      historyBuffer[configDB.currentBuffer].cmdNumber = configDB.currentBuffer;
	  
      returnSymbol=run_command(cmdBuffer, iMode);	  
	  
	  /* These commands cannot be put into the .startup file, it wouldn't make sense */
	  /* Therefore they are only supported in interactive mode                       */
	  switch (returnSymbol) {
	    case (BANG)      : 
                           break;
     	  }
	  returnSymbol = -1;
   }
   return 0;
}

int run_command(char *cmdBuffer, int iMode)
{
  int i;
  char commandString[MAX_LINE];          /* used for temporary command string parseing */
  int cmdToExecute=0;                    /* Used for bangbang command */
  char *argv[64];
  char *pos;
  char *aliasCmd;
  char *token1, *token2, *token3;
  char delims[]=" \"";                   /* Used to tokenize alias command */

  strcpy(commandString,cmdBuffer);
  
  /* returnSymbol=parse_command(cmdBuffer, iMode); */
  
  parse(cmdBuffer, argv, iMode);
  symbol = CMDTOFORK;
  
  if (strcmp(argv[0], "exit") == 0)   
  	      symbol = EXIT;
	if (strcmp(argv[0], "quit") == 0)
  	      symbol = EXIT;
	if (strcmp(argv[0], "bye") == 0) 
  	      symbol = EXIT;
	if (strcmp(argv[0], "exit") == 0)
  	      symbol = EXIT;
	if (strcmp(argv[0], "history") == 0)
  	      symbol = HISTORY;
	if (strcmp(argv[0], "clear") == 0)  
  	      symbol = CLEAR;
	if (strcmp(argv[0], "cls") == 0)  
  	      symbol = CLEAR;
	if (strcmp(argv[0], "help") == 0) 
  	      symbol = HELP;
	if (strcmp(argv[0], "?") == 0)  
  	      symbol = HELP;
	if (strcmp(argv[0], "dumpdb") == 0)
  	      symbol = DUMPDB;
	if (strcmp(argv[0], "endscript") == 0)
  	      symbol = ENDSCRIPT;
	if (strcmp(argv[0], "!!") == 0)
  	      symbol = BANGBANG;
    else {
	   pos=strstr(argv[0], "!");
       if (pos) { symbol = BANG; }
    }
	if (strcmp(argv[0], "script") == 0)
  	      symbol = STARTSCRIPT;
	if (strcmp(argv[0], "alias") == 0)  
  	      symbol = ALIAS;
	if (strcmp(argv[0], "set") == 0)  {
	   if (strcmp(argv[1], "verbose") == 0)
  	      if (strcmp(argv[2], "on") == 0)
  	         symbol = VERBOSEON;
		  if (strcmp(argv[2], "off") == 0)
  	         symbol = VERBOSEOFF;
	   if (strcmp(argv[1], "prompt") == 0) {
  	      symbol = PROMPT;
	   }
	   pos=strstr(commandString, "set path=");
       if (pos) { symbol = SETPATH; }
    }
	if (strcmp(argv[0], "alias") == 0)  
  	      symbol = ALIAS;
	if (strcmp(argv[0], "\n") == 0)  
  	      symbol = NEWLINE;
		  
  /* if (configDB.verbose) printf("Command parsed, symbol = %d.\n",symbol); */
  
  switch (symbol) {
    case (CMDTOFORK): fork_child_process(argv);
	                  if (iMode) configDB.currentBuffer++;
			          break;
    case HISTORY  	: if (iMode) configDB.currentBuffer++;
                      if (iMode) execute_history();     
                      break;
    case PROMPT     : /* if (configDB.verbose) printf("Setting prompt to %s..\n",argv[2]); */
	                  execute_prompt(); 
					  strcpy(configDB.prompt, argv[2]);
                      if (iMode) configDB.currentBuffer++;
                      break;             
    case BANG       : 
                      strcpy(commandString,historyBuffer[configDB.currentBuffer].cmdLine+1);
                      /* if (configDB.verbose) printf("Executing bang command, commandString=%s\n", commandString); */
					  cmdToExecute = atoi(commandString);
                      strcpy(historyBuffer[configDB.currentBuffer].cmdLine, historyBuffer[cmdToExecute].cmdLine);
                      parse(historyBuffer[cmdToExecute].cmdLine, argv, iMode);
					  symbol = run_command(historyBuffer[(cmdToExecute)].cmdLine, iMode);
                      break;
    case BANGBANG   : execute_bangbang(); 
	                  strcpy(historyBuffer[configDB.currentBuffer].cmdLine, historyBuffer[(configDB.currentBuffer-1)].cmdLine);
	                  symbol = run_command(historyBuffer[(configDB.currentBuffer-1)].cmdLine, iMode);
                      break;
    case HELP       : execute_help(); 
                      if (iMode) configDB.currentBuffer++;
                      break;       
    case EXIT       : execute_exit(); 
                      break;       
    case CLEAR      : if (iMode) configDB.currentBuffer++;
	                  execute_cls(); 
                      break;       
    case NEWLINE    : if (configDB.verbose) printf("Newline...\n");
                      break;  	
    case VERBOSEON  : printf("Setting configDB.verbose on...\n");
                      configDB.verbose = TRUE;
                      if (iMode) configDB.currentBuffer++;
                      break;       
    case VERBOSEOFF : myprintf("Setting configDB.verbose off...\n");
                      configDB.verbose = FALSE;
                      if (iMode) configDB.currentBuffer++;
                      break;       
    case STARTSCRIPT: if (configDB.verbose) {myprintf("Starting script"); }
                      configDB.script=TRUE;
				   	  strcpy(configDB.scriptFile, argv[1]);
				      init_script();
                      if (iMode) configDB.currentBuffer++;
                      break;     
	case ENDSCRIPT  : printf("Ending script...\n");
                      configDB.currentBuffer++;
				  	  if (configDB.script) {
			             if (configDB.verbose) printf("Closing file scriptFile.\n"); 
						 fclose(configDB.scriptFilePtr);
		                 configDB.script=FALSE;
						}
						else {
						    myprintf("Warning: No script started...\n");
					  }
                      break;       
    case DUMPDB     : execute_dumpdb();
					  if (iMode) configDB.currentBuffer++;
                      break;
    case SETPATH    : strcpy(delims," ");
	                  token1=strtok(commandString,delims);
                      strcpy(delims,"=");
					  token2=strtok(NULL,delims);
                      token3=strtok(NULL,delims);
	                  strcpy(configDB.path, token3);
		              symbol=SETPATH;
	                  break;
    case ALIAS      : symbol=ALIAS;
	                  token1=strtok(commandString,delims);
                      token2=strtok(NULL,delims);
					  strcpy(delims,"\"");
                      token3=strtok(NULL,delims);
	                  strcpy(configDB.aliases[configDB.numAliases].alias, token2);
					  strcpy(configDB.aliases[configDB.numAliases].actualCmd, token3);
					  if (configDB.verbose) printf("Setting alias %s to %s.\n",configDB.aliases[configDB.numAliases].alias, configDB.aliases[configDB.numAliases].actualCmd);
	                     configDB.numAliases++;
					  break;
	default         : symbol=NEWLINE;
	                  break;
   }
      if (configDB.verbose) printf("Your command symbol is %d, length of string is %d\n",symbol, strlen(cmdBuffer));
  
  return symbol;
}

int warning(int warning)
{
   int i;
   printf("<WARNING #%d>, ", warning);
   if (configDB.script) fprintf(configDB.scriptFilePtr,"<WARNING #%d>, ", warning);
   switch (warning)
   {   case 0 : { printf(", malformed alias\n");
                  if (configDB.script) fprintf(configDB.scriptFilePtr,", malformed alias\n");
				  break;
                }
   }
}
int error(int err)
{
   int i;
   switch (err)
   {   case 0 : { myprintf("Executing exit, goodbye...\n");
                  break;
                }
       case 1 : { myprintf("<ERROR>: HALTING EXECUTION,, File could not be opened\n");
				  break;
                }
       case 2 : { myprintf("<ERROR>: HALTING EXECUTION,, too many tokens on line.\n");
				  break;
				}
	   case 3 : { myprintf("<ERROR>: HALTING EXECUTION,, unexpected token count.\n");
				  break;
				}
	   case 4 : { myprintf("<ERROR>: HALTING EXECUTION,, called init_script when scripting not enabled.\n");
				  break;
				}
	   case 5 : { myprintf("<ERROR>: HALTING EXECUTION,, unable to write to script file in init_script.\n");
				  break;
				}
	   case 6 : { myprintf("<ERROR>: HALTING EXECUTION,, path token to long in tokenize()\n");
				  break;
                }
   }
   printf("Press any key to continue.\n");
   i=getc(stdin);
   exit(err);
}

/*******************************************************************************************************************/
/********   I borrowed this little piece of code from a place on
            stackoverflow, just to assist with safely parsing
            the command line.. It worked much better than scanf alone ****/


static int getLine(char *prmpt, char *buff, size_t sz) {
   int ch, extra;

   // Get line with buffer overrun protection
   if (prmpt != NULL) {
      printf("%s",prmpt);
      fflush(stdout);
   }
  
   if (fgets(buff, sz, stdin) == NULL) {
      return NO_INPUT;
	  printf("No input\n");
   }

   // If it was too long, there'll be no newline.  in that case, we flush
   // to end of line so that excess doesn't affect the next call/

   if (buff[strlen(buff)-1]!='\n') {
      extra=0;
      while (((ch = getchar()) != '\n') && (ch != EOF))
         extra=1;
      return (extra == 1) ? TOO_LONG: OK;
   }

   // Otherwise remove newline and give string back to the caller..
   buff[strlen(buff)-1] ='\0';
   return OK;
}
/****   END OF BORROWED CODE  ****/

void execute_dumpdb()
{  
   int i;
   printf("Dumping configuration database...\n");
   printf("configDB.verbose=");
   if (configDB.verbose) printf("TRUE\n"); else printf("FALSE.\n");
   printf("configDB.script=");
   if (configDB.script) printf("TRUE\n"); else printf("FALSE.\n");
   if (configDB.script) printf("configDB.scriptFile=%s\n", configDB.scriptFile);
   printf("configDB.numAliases=%d\n", configDB.numAliases);
   for (i=0;i<configDB.numAliases;i++) {
      printf("%s is an alias for \"%s\".\n",configDB.aliases[i].alias, configDB.aliases[i].actualCmd);
   }
   printf("configDB.numHelpLines=%d\n", configDB.numHelpLines);
   printf("configDB.path=\"%s\"\n",configDB.path);
   printf("configDB.numStartCmds=%d\n",configDB.numStartCmds);
   printf("Startup configuration filename is %s, contains the following:\n", configDB.startupFile);
   printf("--------------------------------------------\n");
   for (i=0;i<configDB.numStartCmds;i++) 
      fputs(startupBuffer[i].cmdLine, stdout);
   printf("--------------------------------------------\n");
}

int execute_help() 
{
   if (configDB.verbose) printf("Executing help.\n");

   print_helpDB();
   
}
void print_helpDB()
{
   /*int i=0;
   for (i=0;i<=configDB.numHelpLines;i++)
       printf("%s\n", configDB.helpLines[i].helpLine);
   printf("\n");*/
   
   printf("\nShell interpreter version %2.2f developed by David Whipple for CS543 @ Drexel.\n\n", configDB.version);
   printf("You have issued the help or ? command.\n\n");
   printf("The following COMMANDS have been implemented:\n\n");
   printf("   clear or cls              - This clears the screen.\n");
   printf("   help or ?                 - This displays the help screen your seeing now\n");
   printf("   set prompt \"string\"       - Changes the shell prompt to string.\n");
   printf("   history                   - Show history.\n");
   printf("   set verbose on            - This toggles the verbose flag to on.\n");
   printf("   set verbose off           - This toggles the verbose flag off.\n");
   printf("   alias newcmd \"command\"    - Create an alias for newcmd for command.\n");
   printf("   script filename           - Start recording all output to this file.\n");
   printf("   endscript                 - End recording to output.\n");
   printf("   dumpdb                    - Dump configuration database.\n");
   printf("   set path=(./bin /usr/bin) - Change the current path environment variable.\n");
   printf("   exit, quit or bye         - This exits the shell.\n");
   printf("   !!                        - This executes the previous command in history.\n");
   printf("   !#                        - This executes the specific # command in history.\n\n");
}

int execute_cls() 
{
   system("clear");
   if (configDB.verbose) printf("Executed cls.\n");
}
int execute_prompt() 
{
   if (configDB.verbose) printf("Executing prompt.\n");
}

int execute_bang() 
{
   if (configDB.verbose) printf("Executing bang.\n");
}

int execute_bangbang() 
{
   if (configDB.verbose) printf("Executing bangbang, last command was %s.\n",historyBuffer[(configDB.currentBuffer-1)].cmdLine);
}

int execute_exit()
{
   if (configDB.verbose) printf("Exiting gracefully, goodbye!!!\n");
   exit(0);
}

int execute_history()
{
   int i;
   if (configDB.verbose) printf("executing history, current buffer is %d\n", configDB.currentBuffer);
   for (i=1;i<configDB.currentBuffer;i++) {
      printf("%4d\t%s\n",i,historyBuffer[i].cmdLine);
   }
}

int welcome_screen()
{
   printf("dwhipple's cs543 shell interpreter, version %2.2f\n",configDB.version);
}

BOOLEAN file_exists(const char *filename)
{  
   FILE *file;
   if (file = fopen(filename, "r"))    
   {        
      fclose(file);        
	  return TRUE;    
   }    
   return FALSE;
}

void myprintf(char *string) {
   if (configDB.script) {
      fprintf(configDB.scriptFilePtr,"%s",string);
      printf("%s",string);
   }
   else {
      printf("%s",string);
   }
}

void init_script()
{  
   int numBytes=0;
   if (configDB.script) {
      if (file_exists(configDB.scriptFile)) {
	     printf("Warning: File exists, cannot overwrite.\n");
		 configDB.script=FALSE;
	  }
	  else {
	     printf("Creating script file \"%s\".\n",configDB.scriptFile);
	     configDB.scriptFilePtr=fopen(configDB.scriptFile, "w+");
		 if (configDB.scriptFilePtr == NULL) error(5);
		 /*numBytes=fprintf(configDB.scriptFilePtr,"Script started...\n");
		 if (numBytes==EOF) {
		    error(5);
		 }
	     else {
		    printf("Wrote %d bytes.\n",numBytes);
		 }
		 */
	  }
   }
   else {
      error(4);
   }
}
int load_config()
{
   FILE *fp;
   fp=fopen(configDB.startupFile, "r");
   if ( fp != NULL )
   {
     char line [ 128 ]; /* or other suitable maximum line size */

      while ( fgets ( line, sizeof line, fp ) != NULL ) /* read a line */
      {
         /* fputs ( line, stdout ); */ /* write the line */
		 strcpy(startupBuffer[configDB.numStartCmds].cmdLine, line);
		 if (configDB.verbose) fputs(startupBuffer[configDB.numStartCmds].cmdLine, stdout);
		 configDB.numStartCmds++;
      }
	  
      fclose ( fp );
   }
   else
   {
      if (configDB.verbose) printf("No %s file.\n", configDB.startupFile);
   }
   return 0;
}

void init_configDB()
{
   /* This function initializes the global configuration database that is used */
   configDB.version=0.2;
   configDB.verbose=FALSE;                      /* Set the default verbose mode to false */
   strcpy(configDB.prompt, "cs543->");          /* Initialize prompt */
   strcpy(configDB.startupFile, ".cs543rc");    /* Initialize startup filename */
   configDB.numAliases=0;                       /* Initialize Number of Aliases */
   configDB.numStartCmds=0;
   configDB.currentBuffer=1;                    /* Initialize current Buffer */
}

