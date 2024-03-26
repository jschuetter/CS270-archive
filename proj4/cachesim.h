#include <stdint.h>

#ifndef LOGBSIZE
/* Cache blocks are 16 bytes */
#define LOGBSIZE 4
#endif

typedef uint64_t addr_t;

typedef struct {
  addr_t tag;
  uint8_t *block;
  int valid;
  int dirty;
} cacheline_t;

typedef struct {
  cacheline_t *lines;  // array of lines (dynamically allocated)
  void *useinfo;   // for implementing replacement policy
} cacheset_t;

typedef struct hwcache {
  cacheset_t *cache_sets;  // array of sets (dynamically allocated)
  struct hwcache *cache_next; // next level down, or NULL if none
  int cache_E;     // # of lines per set
  int cache_s;     // log_2 # of sets (= # of bits in set index)
  int cache_delay; // unspecified units
} cache_t;

cache_t *cache_create(int s, int E, int delay, cache_t *nextlevel);

int cache_access(cache_t *c, addr_t addr, void *value, int size, int iswrite);

void cache_flush(cache_t *c);

/* This will be provided to you as an ELF file */
extern int mem_access(addr_t addr, void *value, int size, int iswrite);
