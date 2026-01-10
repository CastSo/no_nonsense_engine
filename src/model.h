#pragma once
#include "image_view.h"
#include "vector_math.h"
#include "type.h"

struct Model* read_model_lines(char *file_name);
vector4f normal(vector2f uv);