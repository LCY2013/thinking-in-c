//
// Created by fufeng on 2024/2/2.
//

#ifndef THINKING_IN_C_HELPERS_H
#define THINKING_IN_C_HELPERS_H

#include "structs.h"

int calcFibonacci(int);
int __calcFibTCO(int, int, int);
int __calcFibRecursion(int);
int calcDigits(int);
int retrieveGETQueryIntValByKey(char*, const char*);
void wrapStrFromPTR(char*, size_t, const char*, const char*);
void setupServerSettings(int, const char**, serverSettings*);

#endif //THINKING_IN_C_HELPERS_H
