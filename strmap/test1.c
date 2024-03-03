/* initial test driver for strmap implementation */
#include <stdio.h>
#include "strmapbis.h" /* includes string.h and stdlib.h */

#define NUMTOPUT 400
#define KEYLEN 10
/* buffer is twice the key size */
#define BUFLEN (KEYLEN << 1)
#define SEED 0x270beef

/*  return a random printable character with high-order bit = 0 */
#define ALFALEN 64;
static  char *alfabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
  "123456789*+:";

static int count = 0;
char randchar(void) {
  static union {
    long bits;
    unsigned char bytes[4];
  } u;
  char c;
  if (count==0)
    u.bits = random();
  c = alfabet[u.bytes[count]&0x3f];
  count = (count + 1) & 3;
  return c;
}
    
void randstring(char  *bufp, int len) {
  int i;
  for (i=0; i<len; i++)
    bufp[i] = randchar();
  bufp[i] = 0;
}

static char allkeys[NUMTOPUT][BUFLEN];
  
int main() {
  int j, limit;
  strmap_t *map = strmap_create(MIN_BUCKETS);
  int keypad = (KEYLEN >> 1);
  /* make some random keys */
  srandom(SEED);
  for (j=0; j < NUMTOPUT; j++)
    randstring(allkeys[j],(random()%KEYLEN) + keypad);
  for (j=0; j < NUMTOPUT; j++)
    strmap_put(map,allkeys[j],(void *)j); 

  strmap_dump(map);
  printf("before resizing: capacity = %d, load factor = %f.\n",
	 map->strmap_nbuckets,strmap_getloadfactor(map));
  strmap_resize(map,1.0);
  printf("after resizing w/target 1.0: capacity = %d, load factor = %f.\n",
	 map->strmap_nbuckets,strmap_getloadfactor(map));
  strmap_dump(map);
  /* now remove half of what's there */
  limit = NUMTOPUT>>1;
  for (j=0; j<limit; j++) {
    void *pp = strmap_remove(map,allkeys[j]);
    if  (((int)pp) != j)
      printf("Error: strmap_remove(%s) returned %p (expected %x).\n",
	     allkeys[j],pp,j);
  }
  printf("after removing %d keys,load factor = %f.\n",limit,
	 strmap_getloadfactor(map));
  strmap_resize(map,0.75);
  printf("after resizing (target LF = 0.75),load factor = %f, #buckets=%d.\n",
	 strmap_getloadfactor(map),strmap_getnbuckets(map));
  strmap_dump(map);
  printf("End of test1.c.\n");
  return 0;
}
    
