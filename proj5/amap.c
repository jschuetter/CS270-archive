/************* DO NOT MODIFY THIS FILE *****************/
/*
 * A sorted map of strings to counters.
 * Radix sort based on first char, then linked lists.
 */
#include "amap.h"
#include <assert.h>
#include <string.h>


static void mystrcpy(char *from, char *to) {
  while (*from)
    *to++ = *from++;
  *to = 0;
}

static char *mystrdup(char *k) {
  char *s, *kk;
  int len;
  kk=k;
  for (len=0; *k; k++,len++) ;
  s = malloc(len+1);
  assert(s);
  mystrcpy(kk,s);
  return s;
}

amap_t *amap_create(void) {
  amap_t *m = malloc(sizeof(amap_t));
  assert(m != NULL);
  m->map_lists = (el_t **)calloc(NLISTS,sizeof(el_t *));
  assert(m->map_lists);
  m->map_size = 0;
  m->map_dumpnext = NULL;
  m->map_dumpindx = -1;
  return m;
}

/* Paremeters:
 *  key - INPUT - the key to be searched for
 *  ppp - IN/OUT - *ppp is the address of the bucket;
 *        => **ppp is the address of the first element, if any
 *  np - OUTPUT - will contain pointer to the "next" element in list
 * Return value:
 *  pointer to existing element with the given key, if any; else NULL
 * Upon return:
 *  **ppp == current (=retval);
 *  *ppp points to the bucket OR the sme_next field of the previous element
 *  if found (i.e., retval != NULL) *np == current->sme_next
 *  else *np == current (NULL or next element after the searched-for one)
 * Caller can use *ppp and *np to insert/remove elements as appropriate.
 * Invoke like this: (indx is the hash function output)
 *   smel_t **prev = &(map->amap_buckets[indx]);
 *   smel_t *next;
 *   current = locate(key,&prev,&next);
 */
static el_t *locate(char *key, el_t ***ppp, el_t **np) {
  int rv;
  el_t *current = **ppp;
  /* Walk through until we hit null or find one that key is >= to. */
  /* invariant:  **ppp == current */
  while (current != NULL && (rv = strcmp(current->elt_key,key)) < 0)  {
    *ppp = &current->elt_next;
    current = current->elt_next;
  }
  /* (current == NULL || rv >= 0) && **pp == current */
  if (current) { // list was nonempty && rv >= 0
    if (rv > 0) {  // not found; current is the next element
      *np = current;  // the first key after the given key, if any
      return NULL;
    } else {  // rv==0 => current contains the given key
      *np = current->elt_next;
      return current;
    }
  }
  /* Not found and the given key would be last in the list */
  /* **ppp points to the next field of the last element, any */
  *np = NULL;
  return NULL;
}

/* Preconditions:
 * m points to an amap_t created with amap_create() and unmodified since.
 * key points to a null-terminated char array of lowercase letters.
 * increment is non-negative.
 * Postcondition: m contains a pair with the given key. If it
 *   was in the map before, count is increased by increment.
 */
void amap_incr(amap_t *m, char *key, int increment) {
  assert(m->map_dumpindx < 0);
  el_t **prev;
  el_t *np;
  int indx = key[0] - 'a';
  prev = &m->map_lists[indx];
  el_t *current = locate(key,&prev,&np);
  if (current) { // found - we can ignore prev and np
    current->elt_count += increment;
  } else { // not found - insert new element
    el_t* e = (el_t*) malloc(sizeof(el_t));
    assert(e);
    /* we have to make a copy of the key */
    e->elt_key = mystrdup(key);
    e->elt_count = increment;
    /* np is the next element */
    e->elt_next = np;
    /* (*prev) is the previous element's elt_next field */
    *prev = e;
    m->map_size += 1;
  }
}

/* Get elements from the map for output.
 * We remove each element as it is returned.
 * Preconditions:  strbuf points to a  buffer of at least MAXSTRING bytes.
 *                 count points to an int
 * Postconditions: if nonzero is returned:
 *                 strbuf contains the next null-terminated key of next
 *                   element in strcmp() order.
 *                 countp points to the value associated with that key
 *                   as an int in LITTLE-ENDIAN order.
 * Return value:   1 if an element was copied out
 *                 0 if no more elements
 */
int amap_getnext(amap_t *m, char *strbuf,int *countp) {
  el_t *e;
  if (m->map_dumpindx < 0) { // first call to dump
    m->map_dumpindx++;
  }
  while (m->map_lists[m->map_dumpindx]==NULL) {
    m->map_dumpindx += 1;
    if (m->map_dumpindx==NLISTS)
      break;
  }
  /* m->map_lists[m->map_dumpindx]!=NULL || m->map_dumpindx == NLISTS */
  if (m->map_dumpindx==NLISTS)
    return 0;
  e = m->map_lists[m->map_dumpindx];
  m->map_lists[m->map_dumpindx] = e->elt_next;
  mystrcpy(e->elt_key,strbuf);
  memcpy(countp,&e->elt_count,sizeof(int));
  free(e);
  return 1;
}

/* This is here strictly for debug purposes - not needed for the assignment */
void amap_dump(amap_t *m) {
  int i;
  printf("total elements = %d.\n",m->map_size);
  for (i=0; i<NLISTS; i++) {
    el_t *e = m->map_lists[i];
    if (e) {
      printf("list %d:\n",i);
      do {
	printf("  %s:  %d\n",e->elt_key,e->elt_count);
	e = e->elt_next;
      } while (e);
    }
  }
}  


int amap_getsize(amap_t *m) {
  return m->map_size;
}
