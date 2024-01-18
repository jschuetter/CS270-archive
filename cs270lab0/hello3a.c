#include <stdio.h> // for printf()

int main() {
  char whomtogreet[6] = {'w','o','r','l','d',0};
  printf("Hello, %p!\n",whomtogreet);
  return 0;
}
