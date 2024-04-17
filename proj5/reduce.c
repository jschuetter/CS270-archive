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

  /******* YOUR CODE HERE ****/

  /* first close all pipe ends that we won't be using */

  /* Then read from "my" reducepipe, summing pairs in the map.
   * Note: map is empty at this point.
   * Hint: use  readpair() in aux.c.
   */

  /* Now dump the map counts to the driver pipe.
   * Hint: amap_getnext() returns pairs one-at-a-time, in sorted order.
   * Hit: Use writepair() in aux.c.
   */

  /* Finally, close write end of driverpipe[me], to tell driver I'm done. */

  exit(0);
}    
