#include <stdio.h>
#define FOO(x) (1 + x * x)
int main(void) {
  printf("%d", FOO(1 + 2));
  return 0;
}
