// CS 270 Project 3
// String hash map implementation - with enhancements
// Jacob Schuetter
// Updated 2 March 2024
// All code contained herein has been written by the author
// 
// Version enhancements: 
// - Added getloadfactor function to find current load factor of strmap object
// - Added strmap_resize function to resize strmap object to handle excessive load factors

#include "strmapbis.h"

//Hash function for (deterministically) generating location in map based on key value
//Takes number of buckets in hashmap and to hash
//Returns index in hash map for given key
static int hash (unsigned int numbuckets, char* key) {
	unsigned long rv = 12421;  //Initialize return value to prime number over MAX_BUCKETS
	char curChar = key[0];
	int keyInd = 0;
	while (curChar != '\0') {
		rv *= 23;
		rv = rv ^ curChar;
		keyInd++;
		curChar = key[keyInd];
	}
	return rv%(numbuckets);
}

strmap_t *strmap_create (int numbuckets) {
	//Handle numbuckets values out of range defined in strmap.h
	if (numbuckets > MAX_BUCKETS) numbuckets = MAX_BUCKETS;
	else if (numbuckets < MIN_BUCKETS) numbuckets = MIN_BUCKETS;

	strmap_t *rv = (strmap_t *) malloc(sizeof(strmap_t));
	rv->strmap_size = 0;
	rv->strmap_nbuckets = numbuckets;
	rv->strmap_buckets =  (smel_t **) calloc(numbuckets,sizeof(void *)); //Initialize bucket array according to numbuckets argument

	return rv;
}

void *strmap_put(strmap_t *m, char *key, void *value) {
	//Initialize return variable
	void *rv = NULL;

	//Allocate memory for key string to avoid dereference error
	char *keyPtr = strdup(key);//(char *) malloc(sizeof(key));

	//Allocate new element and initialize with given values
	smel_t *newEl = (smel_t *) malloc(sizeof(smel_t));
	
	newEl->sme_key = keyPtr;
	newEl->sme_value = value;
	
	//Find bucket for new element
	//Check for existing linked list in bucket
	int index = hash(m->strmap_nbuckets, key);
	if (m->strmap_buckets[index] != NULL) { //If bucket already contains elements
		smel_t *curEl = m->strmap_buckets[index];
		//Check for cases where key of curEl is greater than or equal to keyPtr

//EXIT CASE 1: key of curEl is greater than keyPtr (insert keyPtr first)
		if (strcmp(curEl->sme_key, keyPtr) > 0) { 
			m->strmap_buckets[index] = newEl;
			newEl->sme_next = curEl;

			m->strmap_size++;
			return rv;

//EXIT CASE 2: key of curEl matches key of head of list
		} else if (strcmp(curEl->sme_key, keyPtr) == 0) { //In case of matching keys, remove older key and return value
			rv = curEl->sme_value;

			m->strmap_buckets[index] = newEl; //Replace curEl with newEl
			newEl->sme_next = curEl->sme_next;
			free(curEl);
			
			//Do not increment strmap_size - elements cancel out
			return rv;
		}

//EXIT CASE 3: default
		//Walk list to find lexical ordering
		while (curEl->sme_next != NULL && strcmp(curEl->sme_next->sme_key, keyPtr) < 0) { //Compare key of next element with given key
			curEl = curEl->sme_next;
		}

		//Check for matching keys
		if (curEl->sme_next != NULL) {
			smel_t *rmEl = curEl->sme_next;
			if (strcmp(rmEl->sme_key, keyPtr) == 0) {
				//If matching key is found, remove element and decrement strmap_size
				rv = rmEl->sme_value; //Store value to be returned

				curEl->sme_next = rmEl->sme_next;
				free(rmEl);
				m->strmap_size--;
			}
		}

		//Insert newEl between curEl and next element
		newEl->sme_next = curEl->sme_next;
		curEl->sme_next = newEl;

	} else { //If bucket is currently empty
		m->strmap_buckets[index] = newEl;
	}
	
	m->strmap_size++; //Increment strmap_size
	return rv;  //Return previous value of key (if it exists)
}

void *strmap_get(strmap_t *m, char *key) {
	//Find index of key
	int index = hash(m->strmap_nbuckets, key);
//EXIT CASE 1: bucket is empty
//Returns null
	if (m->strmap_buckets[index] == NULL) return NULL;

	//List walk to find element
	smel_t *curEl = m->strmap_buckets[index];
	while (curEl != NULL) {
//EXIT CASE 2: matching element is found
//Returns value of element
		if (strcmp(curEl->sme_key, key)  == 0) {
			void *rv = curEl->sme_value;
			return rv;
		} else curEl = curEl->sme_next;
	}
//EXIT CASE 3: no matching element in found
//Returns null
	return NULL; 
}

void *strmap_remove(strmap_t *m, char *key) {
	//Find index of key
	int index = hash(m->strmap_nbuckets, key);
//EXIT CASE 1: bucket is empty
//Returns null
	if (m->strmap_buckets[index] == NULL) return NULL; //Return null if bucket has no elements
	
	//List walk to find element
	smel_t *curEl = m->strmap_buckets[index];
	smel_t *prevEl = NULL;
	while (curEl != NULL) {
//EXIT CASE 2: element with matching key is found and deleted
//Returns value of element
		if (strcmp(curEl->sme_key, key)  == 0) { //If key matches, remove element
			void *rv = curEl->sme_value; //Get return value
			//Test whether curEl is first in bucket
			if (prevEl != NULL) {
				prevEl->sme_next = curEl->sme_next;
			} else m->strmap_buckets[index] = curEl->sme_next;
			
			free(curEl);
			m->strmap_size--; //Decrement map size
			return rv; //Return element value after element is found and removed
		} else {
			prevEl = curEl;
			curEl = curEl->sme_next;
		}
	}
//EXIT CASE 3: no matching element is found
//Returns null
	return NULL; //Return null if element is not in list
}

int strmap_getsize(strmap_t *m) {
	return m->strmap_size;
}

int strmap_getnbuckets(strmap_t *m) {
	return m->strmap_nbuckets;
}

void strmap_dump(strmap_t *m) {
	//Print total size
	printf("total elements = %i.\n",m->strmap_size);

	//Iterate over all buckets
	for (int i = 0; i < m->strmap_nbuckets; i++) {
		smel_t *curEl = m->strmap_buckets[i];
		//Iterate over all elements in bucket (if non-empty)
		if (curEl != NULL) {
			printf("Bucket %i:\n",i);
			while (curEl != NULL) {
				printf("\t%s->%p\n",curEl->sme_key, curEl->sme_value);
				curEl = curEl->sme_next;
			}
		}
	}	
}

double strmap_getloadfactor(strmap_t *map) {
	double rv = (double)map->strmap_size/map->strmap_nbuckets;
	return rv;
}


//Used in strmap_resize for inserting elements into new bucket array
//Similar to code in strmap_put, with significant changes
//Returns 1 when collision is detected
static int findBucket (smel_t *el, smel_t **buckets, int nbuckets) {

	int index = hash(nbuckets, el->sme_key);
	el->sme_next = NULL; //Set next ptr to null to avoid cross-references to old array layout

	if (buckets[index] != NULL) { //If bucket already contains elements
		smel_t *curEl = buckets[index];

		if (strcmp(curEl->sme_key, el->sme_key) > 0) { //Case 1: insert el at head of list
			buckets[index] = el;
			el->sme_next = curEl;

			return 0;

		} else if (strcmp(curEl->sme_key, el->sme_key) == 0) { //Case 2: el matches element at head of list - should not occur
			printf("Case 2\n");
			printf("COLLISION in strmap_resize:\n");
			printf("%s->%p\n", el->sme_key, el->sme_value);
			return 1;
		}

		//Default case: walk list to find lexical ordering
		while (curEl->sme_next != NULL && strcmp(curEl->sme_next->sme_key, el->sme_key) < 0) { //Compare key of next element with given key
//			printf("CurEl: %s, NextEl: %s\n", curEl->sme_key, curEl->sme_next->sme_key);
			curEl = curEl->sme_next;
//			printf("List walk\n");
		}

	/*	//Check for matching keys
		if (curEl->sme_next != NULL) {
			if (strcmp(curEl->sme_next->sme_key, el->sme_key) == 0) {
				printf("COLLISION in strmap_resize:\n");
				printf("%s->%p\n", el->sme_key, el->sme_value);
				return 1;
			}
		}
*/
		//Insert newEl between curEl and next element
		if (curEl->sme_next != el) { //Condition to check whether curEl->sme_next is already set to el (avoid infinite loop in list walk)
			el->sme_next = curEl->sme_next;
		}
		curEl->sme_next = el;
//		el->sme_next = NULL;
//		if (el->sme_next != NULL) printf("Wrote: %s, sme_next: %s, prev: %s\n", el->sme_key, el->sme_next->sme_key, curEl->sme_key);
//		else printf("Write: %s, prev: %s\n", el->sme_key, curEl->sme_key);

	} else { //If bucket is currently empty
//		printf("Else block\n");
		buckets[index] = el;
	}

	return 0;
	
}

void strmap_resize(strmap_t *map, double targetLF) {
	double curLF = strmap_getloadfactor(map);
	double lfMin = (1-LFSLOP)*targetLF;
	double lfMax = (1+LFSLOP)*targetLF;

	if (curLF < lfMin || curLF > lfMax) {
		//Calculate ideal number of buckets for current size and given load factor
		int idealBuckets = (int)(map->strmap_size/targetLF);

		//Clip idealBuckets if necessary
		if (idealBuckets > MAX_BUCKETS) idealBuckets = MAX_BUCKETS;
		else if (idealBuckets < MIN_BUCKETS) idealBuckets = MIN_BUCKETS; 
//		printf("Ideal buckets: %d; MAX_BUCKETS: %d\n", idealBuckets, MAX_BUCKETS);

		//Create new bucket array
		smel_t **newBuckets = (smel_t **) malloc(idealBuckets * sizeof(smel_t));
		int numElements = 0; //Initialize variable to ensure number of elements remains constant
		for (int i = 0; i < map->strmap_nbuckets; i++) {
			smel_t *curEl = map->strmap_buckets[i];
			//Iterate over all elements in bucket to add to newBuckets
			while (curEl != NULL) {
//				printf("NumElements: %d, nextEl: %s\n", numElements, curEl->sme_key);
				//int index = hash(idealBuckets, curEl->key);
				smel_t *nextEl = curEl->sme_next;			
				int mov = findBucket(curEl, newBuckets, idealBuckets);
				if (mov == 1){
//					printf("LOOP BREAK: COLLISION\n");
					//printf("Duplicate key: %s\n", curEl->sme_key);
					break; //Break loop if collision is detected
				}
				
				curEl = nextEl;
				numElements++;
			}

		}

		smel_t** oldBuckets = map->strmap_buckets;
		map->strmap_buckets = newBuckets;
		newBuckets = NULL;
		free(oldBuckets);
		map->strmap_nbuckets = idealBuckets;

		if (numElements != map->strmap_size) {
			printf("CHANGE IN STRMAP_SIZE\n");
		}
		map->strmap_size = numElements;
	} else return;
}

