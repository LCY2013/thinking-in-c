#include <stdio.h>
#define FOO(x) 1 + x * x
int main(void) {
  printf("%d", 3 * FOO(2));
  return 0;
}
