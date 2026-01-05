#pragma once
#include "type.h"
#include "math.h"
#include "vector_math.h"
#define _USE_MATH_DEFINES

void project(vector3f *v, int width, int height);
void perspective(vector3f *v);
void rotation(vector3f *v, double a);