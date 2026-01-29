#include "util.h"

void fill_n(void *base, const void *fill, size_t nmemb, size_t size) {
    for (char *i = base; i < (char *)base + nmemb * size; i += size)
        memcpy(i, fill, size);
}

void swap_int(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

float clamp(float value, float min, float max) {
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}

double radian(double d) {
    return d * (M_PI/180);
}