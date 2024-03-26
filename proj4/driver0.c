#include <stdio.h>
#include "cachesim.h"

#define MEMSIZE  2048   /* two segments, each half */
/* these do not affect the size of the memory! */
#define LOWSTART 0x20000U
#define HIGHSTART  0x80000U
/* cache parameters */
#define L1s 2
#define L1E 2
#define L1DELAY 7
#define L2s 3
#define L2E 5
#define L2DELAY 30

void init_mem(int memsize, addr_t low, addr_t high);


int main() {
  addr_t a,end;
  int blocksize  = 1<<LOGBSIZE;
  int i;
  unsigned fooey;
  
  init_mem(MEMSIZE, LOWSTART, HIGHSTART);
  /* create a hierarchy of caches */
  cache_t *l2 = cache_create(L2s,L2E,L2DELAY,NULL);
  cache_t *l1 = cache_create(L1s,L1E,L1DELAY,l2);
  /* Reading tests - row-major order.
   * Delay pattern should be 107, then 7's */
  printf("blocksize = %d\n",blocksize);
  end = LOWSTART + (MEMSIZE >> 1);
  for (a=LOWSTART; a < end; a+=blocksize) {
    printf("0x%lx: ",a);
    for (i=0; i<blocksize; i++) {
      int delay = cache_access(l1,a+i,&fooey,1,0);
      printf(" %2.2x (%d), ",fooey,delay);
    }
    putchar('\n');
  }

  end = HIGHSTART + (MEMSIZE >> 1);
  for (a=HIGHSTART; a < end; a+=blocksize) {
    printf("0x%lx: ",a);
    for (i=0; i<blocksize; i++) {
      int delay = cache_access(l1,a+i,&fooey,1,0);
      printf(" %2.2x (%d), ",fooey,delay);
    }
    putchar('\n');
  }

  return 0;
}
