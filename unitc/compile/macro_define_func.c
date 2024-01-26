#include <stdio.h>
#define SAY() printf("Hello, "); printf("world!")
int main(void) {
  int input;
  scanf("%d", &input);  
  if (input > 0)
    SAY();
  return 0;
}
