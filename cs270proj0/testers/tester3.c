/* initial test driver for strmap implementation */
#include <stdio.h>
#include "strmap.h"
#include <time.h>

char *key1 = "string1";
char *key2 = "gobbledygook";
char *key3 = "string2";
char *key4 = "mystring";
char *key5 = "X";
char *key6 = "XX";
char *key7 = "Y";
char *key8 = "abcdefghijklm";
char *key9 = "abcdefghijkl";

int numBuckets = 20;
int numElements = 50;
int rmEl = 10;
int maxKey = 25;
strmap_t *m, *n;

void doput(char *k,void *v) {
  void *rv1 = strmap_put(m,k,v);
  void *rv = strmap_put(n,k,v);
  printf("put(%s,%d) returned %p.\n",k,(int)v,rv);
}
//Generate random keys
void keygen(char* str) {
  int len = rand()%maxKey;
  for( int i = 0; i < len; ++i){
    str[i] = '0' + rand()%72; // starting on '0', ending on '}'
  }
  str [len]='\0';
}

int main() {
  srand(time(NULL));
  printf("Create strmap\n");
  m = strmap_create(numBuckets);
  n = strmap_create(numBuckets);
  printf("Add elements\n");
  doput(key1, (void *)237);
  doput(key7, (void *)"Apples");
  doput(key9, (void *) "{]*%^123klasdf");
  strmap_put(n,key2,(void *) "AAAAAA");
  for (int i = 0; i < numElements; i++) {
	  char randKey[maxKey];
	  keygen(randKey);
	  printf("Keygen\n");
	  char randVal2[maxKey];
	  keygen(randVal2);
	  printf("Putting...\n");
	  doput(randKey, (void *)randVal2);
  }
  strmap_dump(m);
  printf("Map n: \n");
  strmap_dump(n);
  printf("Remove elements\n");
  
  strmap_remove(m,key7);
  strmap_remove(m,key1);
  strmap_dump(m);
  printf("Map n: \n");
  strmap_dump(n);
  return 0;
}
    
