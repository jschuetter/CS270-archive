#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for memset
#include <assert.h>
#include "cachesim.h"

/* You can override these constants at compile time */
/* MEMSIZE *must* be a power of 2 */
#define MEMSIZE  8192   /* two segments, each half */
/* The next two do NOT affect the size of the memory */
/* They can be anything - don't have to be a power of 2. */
#define LOWSTART 0x20000U
#define HIGHSTART  0x80000U

/* cache parameters */
#define L1s 3
#define L1E 5
#define L1DELAY 7


void init_mem(int memsize, addr_t low, addr_t high);

int main() {
  uint8_t  myblock[1<<LOGBSIZE];
  addr_t a,end;
  int blocksize  = 1<<LOGBSIZE;
  int segsize = MEMSIZE >> 1;
  int i;
  int lpb = blocksize/sizeof(long);
  unsigned long fooey;
  int delay;
  
  init_mem(MEMSIZE, LOWSTART, HIGHSTART);
  /* create a hierarchy of caches */
  cache_t *l1 = cache_create(L1s,L1E,L1DELAY,NULL);
  /* Read every byte, to warm up the cache. */
  printf("blocksize = %d\n",blocksize);
  end = HIGHSTART + segsize;
  for (a=HIGHSTART; a < end; a+=blocksize) {
    //    printf("0x%lx: ",a);
    for (i=0; i<lpb; i++) {
      delay = cache_access(l1,a+i,&fooey,sizeof(long),0);
      fooey >>= 1;  // dummy
      //      printf(" %2.2x (%d), ",fooey,delay);
    }
    //    putchar('\n');
  }
  end = LOWSTART + segsize;
  for (a=LOWSTART; a < end; a+=blocksize) {
    //    printf("0x%lx: ",a);
    for (i=0; i<blocksize; i++) {
      delay = cache_access(l1,a+i,&fooey,1,0);
      //      printf(" %2.2x (%d), ",fooey,delay);
    }
    //    putchar('\n');
  }


  /* Now test writing, exercise eviction code. Go back to high half. */
  int maxE  = L1E+2;
  int maxs  = L1s;
  addr_t blockmask = ~(blocksize-1);
  /* XXX must be careful: ensure we stay within the segment */
  addr_t tagincr = 1<<(LOGBSIZE + maxs);
  if ((maxE * tagincr) > segsize) {
    fprintf(stderr,"XXXX tagincr * maxE (0x%lx) exceeds segment size (0x%x)\n",
	    maxE*tagincr,segsize);
    fprintf(stderr,"Please run with a bigger segment\n");
    return 1;
  }
  printf("incrementing address by 0x%lx...\n",tagincr);
  /* start address ensures we won't go past the end of the segment */
  a = (HIGHSTART + segsize) - (maxE * tagincr) - tagincr;
  assert((a + maxE*tagincr) < (HIGHSTART + segsize));
  /* write blocksize 0's at that address */
  memset(myblock,0,blocksize);
  delay = cache_access(l1,a,myblock,blocksize,1);
  printf("Wrote %d 0s at 0x%lx (delay=%d).\n",blocksize,a,delay);
  addr_t aa = a;
  /* Modify enough lines in the same set to evict the first block above */
  myblock[0] = 'w';
  for (i=0; i<maxE; i++) {
    aa += tagincr;
    /* write 'w' at the start of the block */
    delay = cache_access(l1,aa,myblock,1,1);
    printf("wrote 'w' @ 0x%lx. (delay=%d)\n",aa,delay);
  }
  memset(myblock,0xff,blocksize); // fill with 1's
  /* now read memory to verify that the originally-written block of 0's
   * was flushed to memory */
  delay = mem_access(a,myblock,blocksize,0);
  printf("memory at location 0x%lx:\n",a);
  for (i=0; i<blocksize; i++)
    printf(" %2.2x",myblock[i]);
  putchar('\n');
  puts("Correct behavior is for the above block to contain all 0's.");
  puts("Delays SHOULD be 5 @ 77, followed by 3 @ 147.");
  return 0;
}
