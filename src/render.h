#pragma once
#include "config.h"

#include "image_view.h"


void clear(const image_view *color_buffer, const vector4f *color);
void line(int ax, int ay, int bx, int by, image_view *color_buffer, vector4f *color);
void sort_y_coordinates(vector3f* vector, int n);
void triangle_scanline(int ax, int ay, int bx, int by, int cx, int cy, image_view *color_buffer, vector4f *color);
void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz, image_view *color_buffer);

struct Model* read_model_lines(char *file_name);
void render_faces(Model* model, image_view* color_buffer);
void render_wireframe(Model* model, image_view* color_buffer);


vector3f convert_to_ndc(vector3f vec, int width, int height);
