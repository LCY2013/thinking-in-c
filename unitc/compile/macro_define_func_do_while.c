#include <stdio.h>
#define SAY() \
  do { printf("Hello, "); printf("world!"); } while(0)
int main(void) {
  int input;
  scanf("%d", &input);
  if (input > 0)
    SAY();
  return 0;
}
