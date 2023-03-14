#ifndef __CMDLINE_H
#define __CMDLINE_H

class cmdLine {
public:
	cmdLine();
	~cmdLine();
	void parseCmdLine( char* command_line );
	void free_cmdline_args( void );

	/* command line args are put here after calling parseCmdLine */
	int    argc;
	char** argv;
};

#endif /* __CMDLINE_H */