/**************** DO NOT MODIFY THIS FILE *******************/
#ifndef AMAP_H
#define AMAP_H
#include <stdio.h> // for printf, ONLY IN strmap_dump
#include <string.h> // for strcmp
#include <stdlib.h> // for malloc/calloc
#define NLISTS 26
#define MAXSTRING 250

typedef struct amap_elt {
  char *elt_key;
  int elt_count;
  struct amap_elt *elt_next; // next in linked list
} el_t;

typedef struct amap {
  el_t **map_lists;  // array of pointers to elements
  el_t *map_dumpnext;  // next element to dump, initially NULL
  int map_dumpindx;  // bucket from which dump_next is taken, initially 0
  int map_size;    // total number in the map
} amap_t;

/* Create a new map, initialized to empty. */
amap_t *amap_create(void);

/* Set or increase the count of the given element by the given amount.
 * If it was not in the map, insert it.
 */
void amap_incr(amap_t *m, char *key, int increment);

/* return the key and count of next element, if any (map is frozen) */
int amap_getnext(amap_t *m, char *strp, int *countp);

/* return the # of elements in the hashtab */
int amap_getsize(amap_t *m);

/* print out the contents of each bucket -- DEBUG */
void amap_dump(amap_t *m);
#endif
