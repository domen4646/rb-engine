#ifndef COMMON_H
#define COMMON_H

#include "cvector.h"
#include <ccd/ccd.h>
#include <ccd/quat.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ruby.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef bool (*RB_FILELIST_CALLBACK)(char*);

unsigned int get_window_width();
unsigned int get_window_height();

int util_rb_filelist(RB_FILELIST_CALLBACK callback);
bool util_c_str_endswith(char* str, char* end);
void __serr();
void _serr(const char* fname);
int limit(int val, int min, int max);
bool cvector_find_and_remove(cvector_vector_type(VALUE) vec, VALUE val);
void cvector_find_and_remove_all(cvector_vector_type(VALUE) vec, VALUE val);

extern SDL_Window* _game_window;
extern SDL_Renderer* _game_renderer;

// structs
typedef struct _sprite {
    VALUE viewport;
    VALUE texture;
    float degrees;
    bool shown;
    bool ignores_offset;
    bool single_sprite;
    SDL_Rect destination;
    SDL_Rect source;
    unsigned char z_index;
    VALUE __self;
    bool flip_x;
    bool flip_y;
    ccd_quat_t _internal_quaternion;
} SPRITE;

// Viewport also has camera functionality
typedef struct _viewport {
    cvector_vector_type(VALUE) sprites[256];
    int offset_x;
    int offset_y;
    float scale_x;
    float scale_y;
    SDL_Rect viewport_rect;
    bool shown;
    bool force_offset;
} VIEWPORT;


// Contains all needed data for the ruby class texture
typedef struct _texture_data {
    SDL_Texture* texture;   // The SDL texture that is used internally to draw this
    bool owns_data;         // Flag that indicates if object "owns" the texture. if it does it is destroyed when the object is destroyed
    char* error;            // If there is any error when loading the texture, this field will contain said error
    int width;
    int height;
} TEXTURE_DATA;

// Additional ruby classes
extern VALUE _game_sprite_class;
extern VALUE _game_viewport_class;
extern VALUE _game_texture_class;

extern VIEWPORT* _internal_global_viewport;
extern cvector_vector_type(VIEWPORT*) _game_viewports;

void sprite_support(const void* obj, const ccd_vec3_t* dir, ccd_vec3_t* vec);

bool cvector_find_and_remove_viewport(cvector_vector_type(VIEWPORT*) vec, VIEWPORT* val);
void cvector_find_and_remove_all_viewports(cvector_vector_type(VIEWPORT*) vec, VIEWPORT* val);

#endif // COMMON_H
