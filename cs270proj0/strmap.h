#include <stdio.h> // for printf, ONLY IN strmap_dump
#include <string.h> // for strcmp
#define MAX_BUCKETS  10000
#define MIN_BUCKETS  10

typedef struct strmap_e {
  char *sme_key;
  void *sme_value;
  struct strmap_e *sme_next;
} smel_t;

typedef struct sm {
  smel_t **strmap_buckets;  // array of pointers to elements
  unsigned int strmap_size;      // # elements in the table
  unsigned int strmap_nbuckets;  // size of the array
} strmap_t;

/* Create a new hashtab, initialized to empty. */
strmap_t *strmap_create(int numbuckets);

/* Insert an element with the given key and value.
 *  Return the previous value associated with that key, or null if none.
 */
void *strmap_put(strmap_t *m, char *key, void *value);

/* return the value associated with the given key, or null if none */
void *strmap_get(strmap_t *m, char *key);

/* remove the element with the given key and return its value.
   Return null if the hashtab contains no element with the given key */
void *strmap_remove(strmap_t *m, char *key);

/* return the # of elements in the hashtab */
int strmap_getsize(strmap_t *m);

/* return the # of buckets in the hashtab */
int strmap_getnbuckets(strmap_t *m);

/* print out the contents of each bucket */
void strmap_dump(strmap_t *m);
