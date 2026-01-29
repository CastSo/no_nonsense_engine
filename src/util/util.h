#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef UTIL_H

#define UTIL_H

void fill_n(void *base, const void *fill, size_t nmemb, size_t size);
void swap_int(int *a, int *b);
float clamp(float value, float min, float max);
double radian(double d);

#endif