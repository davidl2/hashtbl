#include <process.h>
#include <stdio.h>
#include <time.h>

/* This porgram runs under Borland C++ and allows the timing 
   of another program.  It was originally written just to help
   understand how DOS handles return values from the exit() call. */

int main(int argc, char * argv[], char * env[])
{
    int retval;
    time_t time1,time2;
    fprintf(stderr, "runner: (C) 1996,2003 David Lee Lambert\n");

    if (argc < 2) {
      fprintf(stderr, "runner: no child process specified\n");
      return 63;
    }

    time(&time1);
    retval = spawnvpe(P_WAIT, argv[1], &argv[2], env);
    time(&time2);

    fprintf(stderr,"runner: errorlevel = %d\n", retval);
    fprintf(stderr,"runner: time = %d\n", time2-time1); 
    return(retval);
}
