/* initial test driver for strmap implementation */
#include <stdio.h>
#include "strmap.h"

char *key1 = "string1";
char *key2 = "gobbledygook";
char *key3 = "string2";
char *key4 = "mystring";
char *key5 = "X";
char *key6 = "XX";
char *key7 = "Y";
char *key8 = "abcdefghijklm";
char *key9 = "abcdefghijkl";

strmap_t *m;

void doput(char *k,void *v) {
  void *rv = strmap_put(m,k,v);
  printf("put(%s,%d) returned %p.\n",k,(int)v,rv);
}

int main() {
  printf("Create strmap\n");
  m = strmap_create(101);
  printf("Add elements\n");
  doput(key1,(void *)1);
  doput(key2,(void *)2);
  doput(key3,(void *)3);
  doput(key4,(void *)4);
  doput(key5,(void *)5);
  doput(key6,(void *)6);
  doput(key7,(void *)7);
  doput(key8,(void *)8);
  doput(key9,(void *)9);
  doput(key7,(void *)700);
  doput(key1,(void *)100);
  strmap_dump(m);
  printf("Remove elements\n");
  strmap_remove(m,key7);
  strmap_remove(m,key1);
  strmap_dump(m);
  return 0;
}
    
