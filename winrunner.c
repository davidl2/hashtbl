#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* This porgram runs under Borland C++ and allows the timing 
   of another program.  It was originally written just to help
   understand how DOS handles return values from the exit() call. 

   The code is based on that for "runner".
*/

char * command_line = NULL;
unsigned command_line_size = 0;
unsigned command_line_length = 0;
STARTUPINFO startupinfo = {
  sizeof(STARTUPINFO),
  NULL,
  NULL,
  NULL,
  0, 0, 0, 0, 0, 0, 0, 
  0,
  0, 0, NULL,
  NULL, NULL, NULL };

int main(int argc, char * argv[], char * env[])
{
    int i;
    PROCESS_INFORMATION procinfo;
    FILETIME ftStart,ftEnd,ftSystem,ftUser;
    float tUser,tSys,tReal;
    DWORD exit_code;

    fprintf(stderr, "winrunner: (C) 1996,2003 David Lee Lambert\n");

    if (argc < 2) {
      fprintf(stderr, "winrunner: no child process specified\n");
      return 63;
    }

    command_line = malloc(10);
    command_line_size = 10;
    strcpy(command_line,"");

    // join arguments together
    for (i = 1; i < argc; i++) {
      int leninc = strlen(argv[i]);
      printf(".\n");
      while (command_line_length+leninc+4 > command_line_size) {
	command_line_size = (command_line_size==0 ? 1 
			     : command_line_size * 2);
	command_line = realloc(command_line, command_line_size);
	printf("+\n");
      }
      if (strchr(argv[i],' ')) {
	strcat(command_line,"\"");
	strcat(command_line,argv[i]);
	strcat(command_line,"\" ");
	command_line_length += leninc+3;
      } else {
	strcat(command_line,argv[i]);
	strcat(command_line," ");
	command_line_length += leninc=1;
      }
    }

    fprintf(stderr,"winrunner: executing '%s'\n",command_line);

    if (CreateProcess(NULL, command_line, NULL, NULL, FALSE,
		      0, env, NULL, &startupinfo, &procinfo)
	== FALSE) {
      fprintf(stderr, "winrunner: error %d (%s)\n", 
	      ((unsigned long) GetLastError()));
      return 63;
    }

    fprintf(stderr,"winrunner: spawned process ID %ld\n",
	    procinfo.dwProcessId);
    
    while (GetExitCodeProcess(procinfo.hProcess,&exit_code)
	   && exit_code == STILL_ACTIVE) {
      Sleep(500); }
 
    GetProcessTimes(procinfo.hProcess,
		    &ftStart,&ftEnd,&ftSystem,&ftUser);
    tReal=     ((double)ftEnd.dwHighDateTime)
             - ((double)ftStart.dwHighDateTime)
	     + ((double)ftEnd.dwLowDateTime) * 1e-7
             - ((double)ftStart.dwLowDateTime) * 1e-7 ;
    tSys =  ftSystem.dwHighDateTime+ftSystem.dwLowDateTime*1e-7 ;
    tUser=  ftUser.dwHighDateTime+ftUser.dwLowDateTime*1e-7 ;
      

    fprintf(stderr,"winrunner: errorlevel = %d\n", exit_code);
    fprintf(stderr,"winrunner: user   = %10.6f s\n", tUser);
    fprintf(stderr,"winrunner: system = %10.6f s\n", tSys);
    fprintf(stderr,"winrunner: real   = %10.6f s\n", tReal);    
    return(exit_code);
}

