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

int numBuckets = 1;
int numElements = 0;
int rmEl = 10;
int maxKey = 25;
strmap_t *m;

void doput(char *k,void *v) {
  void *rv = strmap_put(m,k,v);
//  printf("put(%s,%d) returned %p.\n",k,(int)v,rv);
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
  printf("Add elements\n");
  doput(key5, (void *)237);
  doput(key6, (void *)"Apples");
  doput(key7, (void *) "{]*%^123klasdf");
  strmap_dump(m);
  printf("Remove elements\n");
  strmap_remove(m,key5);
  strmap_dump(m);
  doput(key5, (void *) 237);

  strmap_remove(m,key6);
  strmap_dump(m);
  doput(key6, (void *)"Apples");

  strmap_remove(m,key7);
  strmap_dump(m);
  doput(key7, (void *) "askldfjasiowe");

  strmap_remove(m, key5);
  strmap_remove(m, key6);
  strmap_dump(m);

  doput(key6, (void *)"Apples");
  doput(key5, (void *)237);
  strmap_dump(m);
  return 0;
}
    
