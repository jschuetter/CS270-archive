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
  printf("Reduce function\n");

  //Close unneeded pipe ends
  close(driverpipes[me].readfd);
  close(reducepipes[me].writefd);

  char strbuf[MAXSTRING];
  int *cntbuf = malloc(sizeof(int));
  int rv;
  //Read while write end of reducepipe is open
  while (rv = readpair(reducepipes[me].readfd, strbuf, cntbuf) > 0) {
  	//Increment count of string key in map
	amap_incr(map, strbuf, *cntbuf);
	//printf("Pair read in reduce()\n");
  }

  while (rv = amap_getnext(map, strbuf, cntbuf)) {
	//Write pair to driverpipe
	writepair(driverpipes[me].writefd, strbuf, cntbuf);
	printf("Pair written from reduce()\n");
  }
  
  close(driverpipes[me].writefd);
  free(cntbuf);
  printf("Closed driverpipes[%d].writefd and exited reduce process\n", me);

  exit(0);
}    
