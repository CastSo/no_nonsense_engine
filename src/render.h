#pragma once
#include "config.h"

#include "type.h"
#include "image_view.h"


void clear(const image_view *color_buffer, const vector4f *color);
void line(int ax, int ay, int bx, int by, image_view *color_buffer, vector4f *color);
struct Model* read_model_lines(char *file_name);
void render_wireframe(Model* model, image_view* color_buffer);