#pragma once
#include "config.h"

#include "image_view.h"


void clear(const image_view *color_buffer, const vector4f *color);
void line(int ax, int ay, int bx, int by, image_view *color_buffer, vector4f *color);
void sort_y_coordinates(vector3f* vector, int n);
void triangle_scanline(int ax, int ay, int bx, int by, int cx, int cy, image_view *color_buffer, vector4f *color);
void triangle(matrix4f viewport, double *zbuffer, vector4f texture[3], vector4f eye[3], vector4f clip[3], vector4f color, image_view *color_buffer);

struct Model* read_model_lines(char *file_name);
void render_faces(Shader *shader, double *zbuffer, image_view* color_buffer);
void render_wireframe(Model* model, image_view* color_buffer);


vector3f convert_to_ndc(vector3f vec, int width, int height);
