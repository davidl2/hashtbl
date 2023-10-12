#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hashtbl.h"

int main(int argc, char *argv[])
{
  Hashtable table;

  char cmd[2];
  char idbuf[10];
  char valbuf[80];
  char lbuf[132];
#ifdef DEBUG
  long Q;
#endif

  bool found;
  const char * valret;

  while (fgets(lbuf,sizeof(lbuf),stdin)) {
      if (sscanf(lbuf,
		 "%1[i] %9[a-zA-Z0-9_.$]/%79[ -~]",
		 cmd,idbuf,valbuf) == 3 ||
	  sscanf(lbuf,
		 "%1[eq] %9[a-zA-Z0-9_.$]",
		 cmd,idbuf) == 2 
#ifdef DEBUG
	  ||
	  sscanf(lbuf,"%1[d]", cmd) == 1
	  ||
     sscanf(lbuf,"%1[Q] %li", cmd, &Q) == 2
#endif
	  ) {
	switch (cmd[0]) {
	case 'i':
	  found = table.insert(idbuf,valbuf);
	  printf("OK. (%s)\n", found ? "replaced" : "inserted");
	  break;
	case 'e':
	      found = table.erase(idbuf);
	      if (found) {
		  printf("OK.\n");
	      } else {
		  printf("Not found.\n");
	      }
	      break;
	case 'q':
	      valret = table.query(idbuf);
	      if (valret) {
		  printf("%s\n",valret);
	      } else {
		  printf("Not found.\n");
	      }
	      break;
#ifdef DEBUG
   case 'Q':
       if (Q < table.nbuckets) {
         printf("bucket %d/%d: %p  count: %d\n", (int)Q, table.nbuckets,
              table.index[Q].bucket, table.index[Q].count);
       } else {
#ifndef TC1
         Hashtable::data_node * p = (Hashtable::data_node *)(void *) Q;
         printf("key: '%s' value: '%s' prev: %p next: %p\n",
                p->key?p->key:"NULL", p->value?p->value:"NULL",
                p->prev, p->next);
#else /* TC1 : Turbo C++ 1.01 */
         void * p = (void *) Q;
         printf("key: '%s' value: '%s' prev: %p next: %p\n",
                (const char*) (*((void**)p+0) ? *((const char**)p+0):"NULL"),
                (const char*) (*((void**)p+2) ? *((const char**)p+2):"NULL"),
                *((void**)p+4), *((void**)p+6)   );
#endif
       }
       break;
	case 'd':
	    table.dump();
#endif
	}
      }
  }
  return 0;
}

