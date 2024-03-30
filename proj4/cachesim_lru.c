// CacheSim
// CS 270 Project 4
// Jacob Schuetter
// Created 25 Mar 2024
// Modified 30 Mar 2024
//
// All code herein is the work of the author

#include <stdlib.h>
#include <string.h>
#include "cachesim.h"
#include <stdio.h>

extern int mem_access(addr_t addr, void *value, int size, int iswrite);
static void update_useinfo(cache_t *c, cacheset_t *set, int lineNo);

//Constants for min/max address values (for masking)
const static int ADDR_MAX = 0xffffff;
const static int ADDR_MIN = 0x000000;

const static int BLOCK_SIZE = 1 << LOGBSIZE; //Block size in Bytes

//Cache_create
//@param s: (int) log_2 # of sets in cache
//@param E: (int) # of lines per set
//@param delay: (int) length of simulated access delay
//Uses Least Recently Used replacement policy
//*useinfo ptr in each set points to array of ints tracking number of uses of each line in set
cache_t *cache_create(int s, int E, int delay, cache_t *nextlevel) {
	//Allocate new cache type
	cache_t *newCache = malloc(sizeof(cache_t));

	//Allocate memory for new cache set array
	newCache->cache_sets = malloc((1<<s) * sizeof(cacheset_t));

	//Initialize sets
	for (int i = 0; i < (1<<s); i++) {
		//Initialize array of lines
		newCache->cache_sets[i].lines = malloc(E * sizeof(cacheline_t));
		for (int j = 0; j < E; j++) {
			cacheline_t* cacheLine = &(newCache->cache_sets[i].lines[j]); //Define variable for readability
			
			//Initialize cache line
			cacheLine->tag = 0;
			cacheLine->block = calloc(BLOCK_SIZE, sizeof(uint8_t));
			cacheLine->valid = 0;
			cacheLine->dirty = 0;
		}

		//Create array of ints for LRU replacement
		newCache->cache_sets[i].useinfo = (int *)malloc(E * sizeof(int));
		int *setUses = newCache->cache_sets[i].useinfo;
		for (int j = 0; j < E; j++) {
			setUses[j] = j;
		}
	}

	//Fill in data members
	newCache->cache_s = s;
	newCache->cache_E = E;
	newCache->cache_delay = delay;
	newCache->cache_next = nextlevel;
}

//Cache_access
//@param c: (cache_t*) cache to access
//@param addr: (addr_t*) line addr to access
//@param value: (void*) ptr to read destination/write source
//@param size: (int) number of bytes to be transferred (in pwrs of 2)
//@param iswrite: (int) write flag (0=read; !0=write)
//@return int: total delay (from this call and all recursive access calls)
int cache_access (cache_t *c, addr_t addr, void *value, int size, int iswrite) {
	//Initialize return value
	int totalDelay = 0;

	//Find location of addr
	// addr_t is type uint64_t = 64b
	// tag / set / offset
	// ?? bits / s bits / LOGBSIZE bits
	int addrOffset = addr & ((1 << LOGBSIZE) - 1);
	int addrIndex = addr >> LOGBSIZE;
	addrIndex = addrIndex & ((1 << c->cache_s) - 1); //Set bit mask for set index
	addr_t addrTag = addr >> (LOGBSIZE + c->cache_s);

	//Debug print statement
//	printf("%d | %d | %d\n", addrOffset, addrIndex, (int) addrTag);

	cacheset_t *addrSet = &c->cache_sets[addrIndex];
	int addrLineNo = -1; //Store index of line for useinfo

	//Search for matching line in cache set
	for (int i = 0; i < c->cache_E; i++) {
		if (addrSet->lines[i].valid == 1 && addrSet->lines[i].tag == addrTag) {
			addrLineNo = i;
			break;
		}
	}

	//Define addrSet->useInfo as int[]
	int *setUses= addrSet->useinfo;

	//IF LINE WAS FOUND
	if (addrLineNo != -1) {
//		printf("Hit\n");

		//Calculate memory address to be accessed
		uint8_t *memAddr = &(addrSet->lines[addrLineNo].block[addrOffset]);

		//Read or write data, according to value of iswrite
		if (iswrite) {
			memmove(memAddr, value, size); //Check to make sure offset value works
			
			//Set dirty bit
			addrSet->lines[addrLineNo].dirty = 1;

			//Debug print
//			printf("Wrote \"%s\" to address %p in line %d, set %d", (char *) value, memAddr, addrLineNo, addrIndex);
			
		} else {
			memmove(value, memAddr, size);

			//Debug print
//			printf("Read %d bytes from address %p in line %d, set %d", size, memAddr, addrLineNo, addrIndex);
		}
		
		//Update set useinfo -- increment use count
		update_useinfo(c, addrSet, addrLineNo);

		//setUses[addrLineNo] += 1;
		

	} else { //LINE NOT FOUND
//		printf("Miss\n");
	
		//Search next cache layer for element
		//void *retValue = value;

		//Access next lvl of cache or memory
				
		//Get address of full block of memory
		addr_t bitMask = ADDR_MAX;
		bitMask = bitMask ^ ((1<<LOGBSIZE)-1);	
		addr_t blockAddr = addr & bitMask;

		void *blockBuf = calloc(BLOCK_SIZE, sizeof(uint8_t)); //Declare buffer to store value of whole line

		if (c->cache_next != NULL) {
			totalDelay += cache_access(c->cache_next, blockAddr, blockBuf, BLOCK_SIZE, 0);
		} else {
			totalDelay += mem_access(blockAddr, blockBuf, BLOCK_SIZE, 0);
		}

		//Write fetched block to current cache (regardless of operation)

		//Find least used line in set useinfo
//		int leastInd = 0;
/*		for (int i = 1; i < c->cache_E; i++) {
			if ((int) setUses[i] < setUses[leastInd]) {
				leastInd = i;
			}
			if (setUses[leastInd]== 0) break; //Break loop if unused line is found
		}
*/
		cacheline_t *evictLine = &(addrSet->lines[setUses[0]]);

		//Check for write back before evicting
		if (evictLine->dirty) {
			printf("Writeback\n");

//			cache_t *curLvl = c;
//			int curIndex;
//			addr_t curTag;
//			cacheline_t *curLine;
			//Calculate address of evicted line in each cache
			addr_t writeAddr = evictLine->tag << c->cache_s; //Add tag and shift for set index
			writeAddr += addrIndex; //Add set index
			writeAddr = writeAddr << LOGBSIZE; //Shift for offset bits (left empty)

/*//////////////////////
			while (curLvl->cache_next != NULL) {
				curLvl = curLvl->cache_next;

				curIndex = writeAddr >> LOGBSIZE;
				curIndex = curIndex & ((1 << curLvl->cache_s) - 1); //Get set index for current level
				curTag = writeAddr >> (LOGBSIZE + curLvl->cache_s); //Get element tag
				
				int goodWrite = 0; //Flag for successful writeback
				//Write back to next level of cache
				for (int i = 0; i < curLvl->cache_E; i++) {
					curLine = &(curLvl->cache_sets[curIndex].lines[i]);
					if (curLine->valid && curLine->tag == curTag) {
						memmove(curLine->block, evictLine->block, BLOCK_SIZE);
						curLine->dirty = 0; //Reset dirty bit on writeback
						goodWrite = 1;
						totalDelay += curLvl->cache_delay;
						printf("Good write\n");
					}
				}

				if (!goodWrite) fprintf(stderr, "Failed to write back to address %d in cache %p.\n", (int) writeAddr, curLvl); //Print bad writeback flag if necessary
			}
			//Write back to memory
			mem_access(writeAddr, evictLine->block, BLOCK_SIZE, 1);
*///////////////////////

			if (c->cache_next != NULL) totalDelay += cache_access(c->cache_next, writeAddr, evictLine->block, BLOCK_SIZE, 1);
			else totalDelay += mem_access(writeAddr, evictLine->block, BLOCK_SIZE, 1);

/*			while (curLvl->cache_next != NULL) { //Recursively write back until memory level is reached
				curLvl = curLvl->cache_next; 
				totalDelay += cache_access(curLvl, writeAddr, evictLine->block, BLOCK_SIZE, 1);
			}
			totalDelay += mem_access(writeAddr, evictLine->block, BLOCK_SIZE, 1); //Write back to memory
*/
		}

		evictLine->tag = addrTag;
		memmove(evictLine->block, blockBuf, BLOCK_SIZE); //Copy whole line, either read or written
		evictLine->valid = 1;

		//Perform requested read/write operation
		if (iswrite) {
			memmove(&(evictLine->block[addrOffset]), value, size);
		} else {
			memmove(value, &(evictLine->block[addrOffset]), size);
		}

		evictLine->dirty = iswrite; //Set dirty bit if operation was a write
		
		//Reset usage count for all lines???

		update_useinfo(c,addrSet, setUses[0]); 
		//setUses[leastInd] = 1; //Set number of uses for line
		
		free(blockBuf);
	}

	totalDelay += c->cache_delay; //Increment totalDelay
	return totalDelay; //Return value of total delay

}

//Cache_flush
//Sets all valid bits in cache to 0
void cache_flush(cache_t *c) {
	//Iterate over all lines in all sets
	for (int i = 0; i < 1 << c->cache_s; i++) {
		for (int j = 0; j < c->cache_E; j++) {
			cacheline_t *curLine = &(c->cache_sets[i].lines[j]);
			if (curLine->dirty) {
				//Writeback line if dirty
				addr_t writeAddr = curLine->tag << c->cache_s; //Add tag and shift for set index
				writeAddr += i; //Add set index
				writeAddr = writeAddr << LOGBSIZE; //Shift for offset bits (left empty)
				
				cache_access(c->cache_next, writeAddr, curLine->block, BLOCK_SIZE, 1);
			}
			curLine->valid = 0;
		}
	}
}

//Update useinfo by placing newest used line at end
static void update_useinfo (cache_t *c, cacheset_t *set, int lineNo) {
	int *setUses = set->useinfo;
	int i;

	for (i = 0; i < c->cache_E; i++) {
		if (setUses[i] == lineNo) break; //Find position of addrLineNo in line parmutation array
	}
	for (; i+1 < c->cache_E; i++) {
		setUses[i] = setUses[i+1];
	}
	setUses[c->cache_E - 1] = lineNo;
}
