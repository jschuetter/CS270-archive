#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include "amap.h"
#include "aux.h"

/* reducer - reads (word,count) pairs from different scanners via pipes,
 *  uses the map to sum them up.
 * When the pipe is at EOF, use amap_getnext() to dump them one by one
 * into the pipe the driver reads from.
 * The pipes handle flow control and indicate when we finish, so we don't
 * need any other synchronization.
 */


void reduce(int numprocs, int me, amap_t *map,
	    pipe_t *reducepipes, pipe_t *driverpipes) {
  //Close unneeded pipe ends
  close(driverpipes[me].readfd);
  close(reducepipes[me].writefd);

  char strbuf[MAXSTRING];
  int *cntbuf;
  int rv;
  //Read while write end of reducepipe is open
  while (readpair(reducepipes[me].readfd, strbuf, cntbuf) > 0) {
  	//Increment count of string key in map
	amap_incr(map, strbuf, *cntbuf);
  }

  while (amap_getnext(map, strbuf, cntbuf)) {
	//Write pair to driverpipe
	writepair(driverpipes[me].writefd, strbuf, cntbuf);
  }
  /* Finally, close write end of driverpipe[me], to tell driver I'm done. */
  close(driverpipes[me].writefd);

  exit(0);
}    
