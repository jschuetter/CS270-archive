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
	int index = hash(strmap_getnbuckets(m), key);
	if (m->strmap_buckets[index] != NULL) { //If bucket already contains elements
		smel_t *curEl = *strmap_buckets[index];
		while (curEl->sme_next != NULL) curEl = curEl->sme_next;  //Navigate to end of linked list
		curEl->sme_next = newEl;
	} else { //If bucket is currently empty
		strmap_buckets[index] = newEl;
	}

	return rv;  //Return previous value of key (if it exists)
}

void *strmap_get(strmap_t *m, char *key) {
	//Find index of key
	int index = hash(strmap_getnbuckets(m), key);

	//List walk to find element
	smel_t *curEl = m->strmap_buckets[index];
	while (curEl->sme_next != NULL) {
		if (curEl->sme_key == key) {
			void *rv = curEl->sme_key;
			return rv;
		}
	}
	return NULL;
}
