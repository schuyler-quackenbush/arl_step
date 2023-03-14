#if WIN32
#include <windows.h> //for GetModuleFileName
#elif __APPLE__
#include <libproc.h>
#include <unistd.h>
#define _MAX_PATH	1024
#else
platform error
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmdline.h"
#include "utils.h"

/* class constructor */
cmdLine::cmdLine()
{
	;
}

/* class destructor */
cmdLine::~cmdLine()
{
	/* clean up from cmd line malloc() */
	my_free(argv[0]);
	my_free(argv);
}

/* parse command line
 * note: args containing spaces must be enclosed in double-quotes!
 */
void
cmdLine::parseCmdLine( char* command_line )
{
    char*  arg;
    int    index;
    int    len = (int)strlen(command_line);

    // count the arguments
    argc = 1;
    arg  = command_line;
    while (arg - command_line < len) {
		/* skip white space */
		while ( arg[0] != 0 && (arg[0] == ' ' || arg[0] == '\t') ) {
			arg++;
		}
		/* quoted arg */
		if (arg[0] == '"') {
			argc++;
            arg++;
			/* find final quote */
			arg = strchr(arg, '"');
			arg++;
		}
		else {
			/* non-quoted arg */
			argc++;
			/* skip non-white space */
			while ( arg[0] != 0 && arg[0] != ' ' && arg[0] != '\t' ) {
				arg++;
			}
		}
    }    
    
    // tokenize the arguments
    argv = (char**)malloc(argc * sizeof(char*));
    arg = command_line;
    index = 1;
    while (arg - command_line < len) {
		/* skip white space */
		while ( arg[0] != 0 && (arg[0] == ' ' || arg[0] == '\t') ) {
			arg++;
		}
		if (arg[0] == '"') {
			/* quoted arg */
            arg++;
			argv[index] = arg;
			index++;
			/* find final quote */
			arg = strchr(arg, '"');
			/* NULL-terminate arg string */
			*arg = NULL;
			arg++;
		}
		else {
			/* non-quoted arg */
			argv[index] = arg;
			index++;
			/* skip non-white space */
			while ( arg[0] != 0 && arg[0] != ' ' && arg[0] != '\t' ) {
				arg++;
			}
			/* NULL-terminate arg string */
			*arg = NULL;
			arg++;
		}
    }

    // put the program name into argv[0]
	argv[0] = (char*)malloc(_MAX_PATH);
#if WIN32
    GetModuleFileName(NULL, argv[0], _MAX_PATH);
#elif __APPLE__
    pid_t pid; 
    pid = getpid();
    proc_pidpath (pid, argv[0], _MAX_PATH);
#else
	platform error
#endif

}
