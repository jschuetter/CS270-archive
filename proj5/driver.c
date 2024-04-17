#define _GNU_SOURCE
/********** DO NOT MODIFY THIS FILE ******************/
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "amap.h"
#include "aux.h"

/* control ("driver") program for map-reduce application.
 * Set up plumbing, fork reducers, fork scanners (mappers).
 * Then read from each of its input pipes (one per reducer)
 *    in order until it closes, printing out results.
 * Scanners recursively process files in directories, counting
 *    words in each file, then writing its totals to the appropriate
 *    reducer that is responsible for that part of the alphabet.
 *    When all of its words have been written to a reducer pipe,
 *    scanner closes the write end of that pipe and moves to the next.
 * Reducers read from their input pipes, adding up the counts
 *    from each scanner.  When its input pipe reaches EOF (=> all
 *    scanners have finished), the reducer then writes its totals to
 *    its pipe to the driver. 
 *  Each mapper writes each pair in its map to the appropriate reducer.
 *  Reducers keep their own map, combining inputs from different mappers.
 * Phase 3:
 *  Each reducer prints its tokens, then exits.
 */

#define MAXPROC 8

int whichpipe[ALPHABETLEN];

int main(int argc, char *argv[]) {
  int i, seglen, pipeno;
  int nprocs;
  int pipefds[2];
  pipe_t *reducepipes;  // one per reducer
  //   pid_t *pids;      // don't need this
  pipe_t *driverpipes;    // final destination for all pairs
  amap_t *map;

  if (argc < 2) {
    fprintf(stderr,"Usage: %s <file1> <file2> ...\n",argv[0]);
    return 1;
  }

  /* one scanner and one reducer per command-line argument */
  nprocs = argc - 1;
  if (nprocs > MAXPROC) {
    fprintf(stderr,"Too many arguments (max=%d).\n",MAXPROC);
    return 2;
  }

  // initialize the char->pipe map.  Each reducer gets about 1/nprocs
  // of the alphabet.
  seglen = ALPHABETLEN/nprocs;
  i=0;
  pipeno = i/seglen;
  while (pipeno < nprocs && i < ALPHABETLEN) {
    whichpipe[i] = pipeno;
    pipeno = ++i/seglen;
  }
  if (pipeno == nprocs) {
    pipeno -= 1;
    while (i < ALPHABETLEN) {
      whichpipe[i++] = pipeno;
    }
  }

  /* set up plumbing, so everyone has what they need */
  reducepipes = (pipe_t *)calloc(nprocs,sizeof(pipe_t));
  driverpipes = (pipe_t *)calloc(nprocs,sizeof(pipe_t));
  for (i=0; i<nprocs; i++) {
    if (pipe2(pipefds,O_DIRECT) < 0)
      fail("main pipe2 reducepipes");
    reducepipes[i].readfd = pipefds[0];
    reducepipes[i].writefd = pipefds[1];
    if (pipe2(pipefds,O_DIRECT) < 0)
      fail("main pipe2 driverpipes");
    driverpipes[i].readfd = pipefds[0];
    driverpipes[i].writefd = pipefds[1];
  }


  /* create a map.  Everyone will use (their own copy of) this one  */
  map = amap_create();
  
  /* now create the processes that will do the work */
  for (i=0; i<nprocs; i++) {
    /* create reducers first */
    pid_t pid  = fork();
    if (pid < 0)
      fail("driver: fork reduce");
    else if (!pid) {  // child
      reduce(nprocs,i,map,reducepipes,driverpipes);  // does not return
      /* NOTREACHED */
    }
    /* now create scanners */
    pid = fork();
    if (pid < 0)
      fail("driver: fork scanner");
    else if (!pid) { // child
      /* close driverpipes, scanner doesn't need them */
      for (int j=0; j<nprocs; j++) {
	close(driverpipes[j].readfd);
	close(driverpipes[j].writefd);
      }
      scanner(nprocs,map,argv[i+1],reducepipes); // does not return
      /* NOTREACHED */
    }
  }

  /* parent/driver:
   *  close ALL pipe write ends
   *  close reduce pipe read ends
   */
  for (i=0; i<nprocs; i++) {
    close(reducepipes[i].writefd);
    close(driverpipes[i].writefd);
    close(reducepipes[i].readfd);
  }

  /* read from each driver pipe in order until EOF */
  for (i=0; i<nprocs; i++) {
    int count;
    char str[MAXSTRING];
    while (readpair(driverpipes[i].readfd,str,&count) > 0)
      printf("%s: %d\n",str,count);
  }
  return 0;
}
    
