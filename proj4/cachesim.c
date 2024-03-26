// CacheSim
// CS 270 Project 4
// Jacob Schuetter
// Created 25 Mar 2024
// Modified 25 Mar 2024
//
// All code herein is the work of the author

#include <stdlib.h>
#include "cachesim.h"

//Cache_create
//@param s: (int) log_2 # of sets in cache
//@param E: (int) # of lines per set
//@param delay: (int) length of simulated access delay
//Uses Least Frequently Used replacement policy
//*useinfo ptr in each set points to array of ints tracking number of uses of each line in set
cache_t *cache_create(int s, int E, int delay, cache_t *nextlevel) {
	//Allocate new cache type
	cache_t *newCache = malloc(sizeof(cache_t));

	//Fill in data members
	newCache->cache_s = s;
	newCache->cache_E = E;
	newCache->cache_delay = delay;
	newCache->cache_next = nextlevel;

	//Allocate memory for new cache set array
	newCache->cache_sets = malloc((2^s) * sizeof(cacheset_t));

	//Initialize sets
	for (int i = 0; i < 2^s; i++) {
		//Initialize array of lines
		newCache->cache_sets[i].lines = malloc(E * sizeof(cacheline_t));
		for (int j = 0; j < E; j++) {
			cacheline_t cacheLine = newCache->cache_sets[i].lines[i]; //Define variable for readability
			
			//Initialize cache line
			cacheLine.tag = 0;
			cacheLine.block = calloc(2^LOGBSIZE, sizeof(uint8_t));
			cacheLine.valid = 0;
			cacheLine.dirty = 0;
		}

		//Create array of ints for LFU replacement
		newCache->cache_sets[i].useinfo = calloc(E, sizeof(int));
	}
}
