#include <stdio.h> // for printf()

int main() {
  char whomtogreet[6] = {'w','o','r','l','d',0};
  printf("Hello, %s!\n",whomtogreet);
  whomtogreet[3] = 'k';
  whomtogreet[4] = 0x73;
  printf("Hello, %s!\n",whomtogreet);
  return 0;
}
