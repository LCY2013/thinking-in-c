#include <stdio.h>
#define FOO(x) (1 + (x) * (x))
int main(void) {
  int i = 1;
  printf("%d", FOO(++i));
  return 0;
}
