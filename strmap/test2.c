/* Test driver for strmapbis (Project 3)
 * Checking:
 *  - key bytes with high bit set (sign extension in hash function))
 *  - very long and empty key strings
 *  - removal and re-insertion
 *  - map does not change when load factor is already within range
 *  - resize works correctly
 * Check that get() and remove return correct values.
 */
#include <stdio.h>
#include <ctype.h>  // for isprint() and isspace()
#include "strmapbis.h" // includes string.h and stdlib.h

#define SEED 0xadef7891  // for reproducibility
#define HUGESTRING 2932
#define RANDKEYLEN 9
#define MIDSTRING 24  // ample for printf("0x%lx")
#define REGCASES 15
#define MIDCASES (REGCASES+15)
#define ALLCASES (MIDCASES+15)
#define MAXKEYCHARS 60
#define LOWTARGET 0.1
#define HITARGET 4.0
#define LOBOUND(x) ((1.0-LFSLOP)*(x))
#define HIBOUND(x) ((1.0+LFSLOP)*(x))

static  char *alfabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
  "1234567890-.";

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

/* precondition: bufp points to an array of at least len+1 bytes */
void randstring(char  *bufp, int len) {
  int i;
  for (i=0; i<len; i++)
    *bufp++ = randchar();
  *bufp = 0;
}

/* create a key of given length that may contain "negative" bytes */
/* precondition: bufp points to an array of at least len+1 bytes */
void randkey(char *bufp, int len) {
  int i;
  for (i=0; i<len; i++)
    *bufp++ = random()&0xff;
  *bufp = 0;
}

static char *hexdigs="0123456789abcdef";
/* print a key, escaping non-printable chars and spaces so we don't
 * get ugly output or hose the terminal window.
 * Precondition: k points to a null-terminated sequence of bytes.
 */
void printkey(char *k) {
  int outcnt = 0;
  while (*k && outcnt < MAXKEYCHARS) {
    unsigned char c = *k;
    if (!isprint(c) || isspace(c)) {
      putchar('[');
      putchar(hexdigs[(c>>4)&0xf]);
      putchar(hexdigs[c&0xf]);
      putchar(']');
      outcnt += 4;
    } else {
      putchar(*k);
      outcnt += 1;
    }
    k++;
  }
  if (*k != 0)
    printf("...");
}	      

int main() {
  int i;
  int rv;
  int tablesize;
  char *keys[ALLCASES];
  void *values[ALLCASES];
  void *retval;  // return value in test cases
  strmap_t *map1, *map2;
  double lf;
  
  srandom(SEED);

  /* Create (key,value) pairs in advance, deterministically */
  keys[0] = ""; values[0] = NULL;  // empty string maps to NULL
  /* XXX This is a problem with map semantics as specified. */
  
  keys[1] = malloc(HUGESTRING+1);
  
  randstring(keys[1],HUGESTRING);  values[1] = (void *)1;
  
  /* pairs of random-byte keys, random values */
  for (i=2; i<REGCASES; i++) {
    long temp = (long)random();
    keys[i] = malloc(RANDKEYLEN+1);
    randkey(keys[i],RANDKEYLEN);
    values[i] = (void *)((temp << 32) | random());
  }

  /* pairs with key = hex rendering of value */
  for (i=REGCASES; i<MIDCASES; i++) {
    long temp = random();
    values[i] = (void *)((temp << 32) | random());
    keys[i] = malloc(MIDSTRING);
    snprintf(keys[i],MIDSTRING,"%p",values[i]);
  }

  /* pairs with key and value = same random string */
  for (i=MIDCASES; i<ALLCASES; i++) {
    keys[i] = malloc(RANDKEYLEN+1);
    randstring(keys[i],RANDKEYLEN);
    values[i] = keys[i];
  }

  /* get a couple of maps */
  map1 = strmap_create(MIN_BUCKETS);
  map2 = strmap_create(MIN_BUCKETS<<3); // 80

  /* insert the pairs into both maps */
  for (i=0; i<ALLCASES; i++) {
    retval = strmap_put(map1,keys[i],values[i]);
    if (retval != NULL) {
      printf("Error: put() returned non-null (%p) on new key (",retval);
      printkey(keys[i]);
      printf(") in map1.\n");
    }
    retval = strmap_put(map2,keys[i],values[i]);
    if (retval != NULL)  {
      printf("Error: put() returned non-null (%p) on new key (", retval);
      printkey(keys[i]);
      printf(") in map2.\n");
    }
  }

  printf("****All pairs inserted; "
	 "map1 load factor = %.2f, map2 lf = %.2f.\n",
	 strmap_getloadfactor(map1),strmap_getloadfactor(map2));
  fflush(stdout);

  /* verify that get() returns the correct value */
  for (i=0; i<ALLCASES; i++) {
    retval = strmap_get(map1,keys[i]);
    if (retval != values[i]) {
      printf("Error: get(");
      printkey(keys[i]);
      printf(")\n   returned %p. Expected %p.\n",retval,values[i]);
    }
  }
  /* verify that get() returns NULL for a key not in themap */
    retval = strmap_get(map1,"this key is not in the map");
  if (retval != NULL)
    printf("Error: get() returned non-null (%p) for a key not in the map.\n",
	   retval);

  printf("****Finished get() tests.\n");
  fflush(stdout);

  /* verify that each map contains as many pairs as we think */
  rv=strmap_getsize(map1);
  if (rv != ALLCASES)
    printf("Error: getsize(map1) returned %d, expected %d.\n",rv,ALLCASES);

  rv=strmap_getsize(map2);
  if (rv != ALLCASES)
    printf("Error: getsize(map2) returned %d, expected %d.\n",rv,ALLCASES);
  
  /* verify that the size of the map won't be changed if the load
   * factor is already in range.  map1 LF should be ALLCASES/MINBUCKETS.
   * XXX change these constants if ALLCASES or MIN_BUCKETS changes.
   * Acceptable range for 45 and 10 should be 3.9375 to 5.0625.
   */
  rv = strmap_getnbuckets(map1);
  strmap_resize(map1,4.0);
  tablesize = strmap_getnbuckets(map1);
  if (tablesize != MIN_BUCKETS)
    printf("Error: resize(map1,4.0) changed map. "
	   "Old size=%d, new size=%d.\n",rv,tablesize);
  /* XXX if they fail the previous test, nbuckets has changed, so
   * they will likely fail this one also. */
  strmap_resize(map1,5.0);
  tablesize = strmap_getnbuckets(map1);
  if (tablesize != MIN_BUCKETS)
    printf("Error: resize(map1,5.0) changed map. "
	   "Old size=%d, new size=%d.\n",rv,tablesize);

  rv = strmap_getnbuckets(map2);
  strmap_resize(map2,0.5); // should be within range
  tablesize = strmap_getnbuckets(map2);
  if (tablesize != rv)
    printf("Error: resize(map2,0.5) changed map. "
	   "Old size=%d, new size=%d.\n",rv,tablesize);
  strmap_resize(map2,0.6); // still within range
  tablesize = strmap_getnbuckets(map2);
  if (tablesize != rv)
    printf("Error: resize(map2,0.6) changed map. "
	   "Old size=%d, new size=%d.\n",rv,tablesize);
  
  printf("****Finished redundant resize() test.\n");
  fflush(stdout);
  
  /* Now resize map1 to a much bigger capacity and see that it's right */
  strmap_resize(map1,LOWTARGET);
  tablesize = strmap_getnbuckets(map1);
  if ((tablesize < MIN_BUCKETS) ||
      (tablesize > MAX_BUCKETS))
    printf("Error: after resize(%.2f) map1 # buckets (%d) outside limits.\n",
	   LOWTARGET,tablesize);
  lf = strmap_getloadfactor(map1);
  if ((lf < LOBOUND(LOWTARGET)) ||
      (lf > HIBOUND(LOWTARGET)))
    printf("Error: after resize(%.2f), map1 load factor (%.2f) "
	   "is out of range.\n",LOWTARGET,lf);

  /* resize map2 to make it smaller */
  strmap_resize(map2,HITARGET);
  tablesize = strmap_getnbuckets(map2);
  if ((tablesize < MIN_BUCKETS) ||
      (tablesize > MAX_BUCKETS))
    printf("Error: after resize(%.2f) map2 # buckets (%d) outside limits.\n",
	   HITARGET,tablesize);
  lf = strmap_getloadfactor(map2);
  if ((lf < LOBOUND(HITARGET)) ||
      (lf > HIBOUND(HITARGET)))
    printf("Error: after resize(%.2f), map2 load factor (%.2f) "
	   "is out of range.\n",HITARGET,lf);

  
  printf("****Finished resize tests. "
	 "Current # buckets: map1=%d, map2=%d.\n",
	 strmap_getnbuckets(map1),strmap_getnbuckets(map2));
  fflush(stdout);

  /* Now remove everything from map2, checking the values returned. */
  for (i=0; i<ALLCASES; i++) {
    retval = strmap_remove(map2,keys[i]);
    if (retval != values[i]) {
      printf("Error: remove(");
      printkey(keys[i]);
      printf(")\n  returned %p. Expected %p.\n",retval,values[i]);
    }
  }
  /* verify emptiness */
  rv = strmap_getsize(map2);
  if (rv != 0)
    printf("Error: map2 size after removing all keys was nonzero: %d.\n",rv);

  printf("****Finished remove tests.\n");
  fflush(stdout);
  
  /* put everything back in the empty map */
  for (i=0; i<ALLCASES; i++) {
    retval = strmap_put(map2,keys[i],values[i]);
    if (retval != NULL) {
      printf("Error: re-inserting (");
      printkey(keys[i]);
      printf(",%d)\n    into empty map2 returned %p (expected 0x0).\n",
	     i,retval);
    }
  }

  /* for the pairs where key = hex rendering of value, add 1 to value */
  for (i=REGCASES; i<MIDCASES; i++) {
    unsigned long x = (unsigned long)values[i];
    retval = strmap_put(map1,keys[i],(void *)(x+1));
    if (retval != values[i])
      printf("Error: put(%s,...) returned wrong value:\n"
	     "   expected %p, got %p.\n",keys[i],values[i],retval);
    retval=strmap_remove(map1,keys[i]);
    if (retval != ((void *)x+1))
      printf("Error: remove(%s) returned wrong value:\n"
	     "   expected %p, got %p.\n",keys[i],(void *)x+1,retval);	     
  }

  printf("****Finished change value of existing key test.\n");
  fflush(stdout);

#if 0
  for (i=0; i<MIDCASES; i++) {
    printkey(keys[i]);
    printf("-> %p\n",values[i]);
  }
  for (i=MIDCASES; i<ALLCASES; i++)
    printf("%s -> %s\n",keys[i],(char *)values[i]);
#endif  

  printf("End of test2.\n");
  return 0;
}
    
