/************ DO NOT CHANGE THIS FILE ******************/
#include "amap.h"
#define ALPHABETLEN 26

typedef struct ppair {
  int readfd;
  int writefd;
} pipe_t;

void fail(char *s1);  // DOES NOT RETURN
int readpair(int fd, char *strbuf, int *count);
int writepair(int fd, char *strbuf, int *count);
void reduce(int numprocs, int me, amap_t *map, pipe_t *reducepipes,
		   pipe_t *driverpipes);
void scanner(int numprocs, amap_t *map, char *name, pipe_t *rpipes);

