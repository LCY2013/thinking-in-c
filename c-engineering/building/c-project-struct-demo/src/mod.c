//
// Created by fufeng on 2024/2/1.
//

int fib(int n) {
    if (n <= 1) {
        return n;
    }
    return fib(n-1) + fib(n-2);
}
