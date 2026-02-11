#pragma once
#include "config.h"

#include "image_view.h"
#include "model.h"
#include "./microui/atlas.inl"

typedef struct Edge {
    //Pixel group dimensions
    int step_x_size;
    int step_y_size;

    //stores 4 signed int in SIMD registers
    vector4f one_step_x;
    vector4f one_step_y;
}Edge;

void clear(const image_view *color_buffer, const vector4f *color);
void render_tga(image_view *color_buffer, image_view *img_buffer);

void line(int ax, int ay, int bx, int by, image_view *color_buffer, color4ub color);
void sort_y_coordinates(vector3f* vector, int n);

vector4f edge_init(Edge* self, vector2f v0, vector2f v1, vector2f origin);
float twice_triangle_area(vector2f a, vector2f b, const vector2f c);
void triangle2D(image_view* color_buffer, vector3f clip[3], color4ub color, bool is_backface_cull);
float signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy);
void triangle2D_texture(image_view* color_buffer, vector3f clip[3], vector3f tex[3], color4ub color, bool is_backface_cull);
void triangle3D(Shader *shader,  Model *model, float *zbuffer,  float* depth_buffer, image_view *color_buffer,  bool is_backface_cull);
void render_pixel(Shader* shader, Model* model, float *zbuffer, float* depth_buffer, image_view* color_buffer, float x, float y, vector3f barycoord);

void render_faces(Shader *shader, Model *model, float *zbuffer, float* depth_buffer, image_view* color_buffer,  bool is_bf_cull);
void render_wireframe(Model* model, image_view* color_buffer);

void render_gui_texture(image_view* color_buffer, mu_Rect dst, mu_Rect src, mu_Color color);
void render_gui(image_view* color_buffer, mu_Rect dst, mu_Rect src, mu_Color color);


