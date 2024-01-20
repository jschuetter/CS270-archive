#include "strmap.h"
#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>

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
//	if (rv->**strmap_buckets == NULL) {
//		perror("malloc");
//		exit(0);
//	}

	return rv;
}

void *strmap_put(strmap_t *m, char *key, void *value) {
	//Check for previous value at given key
	void *rv = strmap_get(m, key);

	//Find bucket for new element
	//Check for existing linked list in bucket
	int index = hash(strmap_getnbuckets(m), key);
	if (m->strmap_buckets[index] != NULL) {
		
	}

	//Allocate new element and initialize with given values
	smel_t* newEl = (smel_t *) malloc(sizeof(smel_t));
	
	newEl->sme_key = key;
	newEl->sme_value = value;
	

}
