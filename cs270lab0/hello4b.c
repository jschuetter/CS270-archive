#include <stdio.h> // for printf()

int main() {
  char whomtogreet[6] = "world";
  printf("Hello, %s!\n",whomtogreet);
  whomtogreet[3] = 'k';
  whomtogreet[4] = 0x73;
  printf("Hello, %s!\n",whomtogreet);
  return 0;
}
