#include "strmap.h"
#include <stdlib.h>

//Hash function for (deterministically) generating location in map based on key value
//@param key: string value of key
//@return int: index in hash map for given key
int hash (unsigned int numbuckets, char* key) {
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

//Check malloc returns - returns true if valid, exits if not
void retcheck(void *ptr) {
	if (ptr == NULL) {
		perror("malloc");
		exit(0);
	}
}

strmap_t *strmap_create (int numbuckets) {
	//Handle numbuckets values out of range defined in strmap.h
	if (numbuckets > MAX_BUCKETS) numbuckets = MAX_BUCKETS;
	else if (numbuckets < MIN_BUCKETS) numbuckets = MIN_BUCKETS;

	strmap_t *rv = (strmap_t *) malloc(sizeof(strmap_t));
	retcheck(rv);
	rv->strmap_size = 0;
	rv->strmap_nbuckets = numbuckets;
	rv->strmap_buckets =  (smel_t **) calloc(numbuckets,sizeof(void *)); //Initialize bucket array according to numbuckets argument
	retcheck(rv->strmap_buckets);

	return rv;
}

void *strmap_put(strmap_t *m, char *key, void *value) {
	//Allocate memory for key string to avoid dereference error
	char *keyPtr = (char *) malloc(sizeof(key));
	keyPtr = key;

	//Check for previous value at given key
	void *rv = strmap_get(m, key);

	//Allocate new element and initialize with given values
	smel_t *newEl = (smel_t *) malloc(sizeof(smel_t));
	
	newEl->sme_key = keyPtr;
	newEl->sme_value = value;
	newEl->sme_next = NULL;
	
	//Find bucket for new element
	//Check for existing linked list in bucket
	int index = hash(m->strmap_nbuckets, key);
	if (m->strmap_buckets[index] != NULL) { //If bucket already contains elements
		smel_t *curEl = *strmap_buckets[index];
		char **nextKey = curEl->sme_next->sme_key; //Get key of next element
		while (curEl->sme_next != NULL && strcmp(nextKey, keyPtr) < 0) { //Walk list to find lexical ordering
			curEl = curEl->sme_next;
			nextKey = curEl->sme_next->sme_key;
		}
		//Insert newEl between curEl and next element
		newEl->sme_next = curEl->sme_next;
		curEl->sme_next = newEl;
	} else { //If bucket is currently empty
		strmap_buckets[index] = newEl;
	}

	return rv;  //Return previous value of key (if it exists)
}

void *strmap_get(strmap_t *m, char *key) {
	//Find index of key
	int index = hash(m->strmap_nbuckets, key);

	//List walk to find element
	smel_t *curEl = m->strmap_buckets[index];
	while (curEl->sme_next != NULL) {
		if (strcmp(curEl->sme_key, key)  == 0) {
			void *rv = curEl->sme_value;
			return rv;
		} else curEl = curEl->sme_next;
	}
	return NULL;
}

void *strmap_remove(strmap_t *m, char *key) {
	//Find index of key
	int index = hash(m->strmap_nbuckets, key);
	if (m->strmap_buckets[index] == NULL) return NULL; //Return null if bucket has no elements
	
	//List walk to find element
	smel_t *curEl = m->strmap_buckets[index];
	smel_t *prevEl = NULL;
	while (curEl->sme_next != NULL) {
		if (strcmp(curEl->sme_key, key)  == 0) { //If key matches, remove element
			void *rv = curEl->sme_value; //Get return value
			if (prevEl != NULL) {
				prevEl->sme_next = curEl->sme_next;
			} else m->strmap_buckets[index] = curEl->sme_next;
			return rv; //Return element value after element is found and removed
		} else {
			prevEl = curEl;
			curEl = curEl->sme_next;
		}
	}
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
			while (curEl->sme_next != NULL) {
				printf("\t%s->%p\n",curEl->sme_key, curEl->sme_value);
			}
		}
	}	
}
