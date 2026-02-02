#pragma once
#include "config.h"

#include "image_view.h"
#include "model.h"
#include "./microui/atlas.inl"

void clear(const image_view *color_buffer, const vector4f *color);
void render_tga(image_view *color_buffer, image_view *img_buffer);

void line(int ax, int ay, int bx, int by, image_view *color_buffer, vector4f *color);
void sort_y_coordinates(vector3f* vector, int n);
void triangle_scanline(int ax, int ay, int bx, int by, int cx, int cy, image_view *color_buffer, vector4f *color);
void triangle2D(image_view* color_buffer, vector3f clip[3], color4ub color, bool is_backface_cull);
double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy);
void triangle2D_texture(image_view* color_buffer, vector3f v[3], vector3f tex[3], color4ub color, bool is_backface_cull);
void triangle3D(Shader *shader,  Model *model, double *zbuffer,  vector4f clip[3], vector3f varying_uv[3], vector4f norm[3], image_view *color_buffer,  bool is_backface_cull);

struct Model* read_model_lines(char *file_name);
void render_faces(Shader *shader, Model *model, double *zbuffer, image_view* color_buffer,  bool is_bf_cull, float move);
void render_wireframe(Model* model, image_view* color_buffer);

void render_gui_texture(image_view* color_buffer, mu_Rect dst, mu_Rect src, mu_Color color);
void render_gui(image_view* color_buffer, mu_Rect dst, mu_Rect src, mu_Color color);

vector3f convert_to_ndc(vector3f vec, int width, int height);
