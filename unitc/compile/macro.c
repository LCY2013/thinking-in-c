#pragma GCC warning "Just FYI!"
#include <stdbool.h>
#define PI 3.14
#define SQUARE(x) (x * x)
int main(void) {
#if defined PI
    // Some specific calculations.
    const double area = SQUARE(5) * PI;
    const bool isAreaGT100 = area > 100.0;
#endif
    return 0;
}
