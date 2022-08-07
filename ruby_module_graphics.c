#include "rubyutils.h"

void _clear_black();
void _clear_white();
void _clear_red();
void _present();
void _update();
VALUE _get_fps();


void rubyutil_register_graphics_module() {

    VALUE graphics_module = rb_define_module("Graphics");

    rb_define_module_function(graphics_module, "clear", _clear_black, 0);
    rb_define_module_function(graphics_module, "clear_white", _clear_white, 0);
    rb_define_module_function(graphics_module, "clear_red", _clear_red, 0);
    rb_define_module_function(graphics_module, "display", _present, 0);
    rb_define_module_function(graphics_module, "update", _update, 0);
    rb_define_module_function(graphics_module, "fps", _get_fps, 0);

}

void _clear_black() {
    SDL_SetRenderDrawColor(_game_renderer, 0, 0, 0, 255);
    SDL_RenderClear(_game_renderer);
}

void _clear_white() {
    SDL_SetRenderDrawColor(_game_renderer, 255, 255, 255, 255);
    SDL_RenderClear(_game_renderer);
}

void _clear_red() {
    SDL_SetRenderDrawColor(_game_renderer, 255, 0, 0, 255);
    SDL_RenderClear(_game_renderer);
}

void _present() {
    SDL_RenderPresent(_game_renderer);
}

Uint32 _last_tick = 0;
float _fps = 0.0F;

VALUE _get_fps() {
    return INT2FIX((int)_fps);
}

void _update() {
    //printf("len of [%p] => %i\n", _game_viewports[0]->sprites[0], cvector_size(_game_viewports[0]->sprites[0]));

    // Measure
    Uint32 diff = SDL_GetTicks() - _last_tick;
    _last_tick += diff;
    if (diff <= 0)
        diff = 1;
    _fps = 1000.0F / (float)diff;

    // Clear
    SDL_SetRenderDrawColor(_game_renderer, 0, 0, 0, 255);
    SDL_RenderClear(_game_renderer);

    // Draw
    game_viewport_draw(_internal_global_viewport);
    for (int i=0; i<cvector_size(_game_viewports); i++) {
        //printf("[INFO] len of {%p} [%p] => %i\n", _game_viewports[i], _game_viewports[i]->sprites[0], cvector_size(_game_viewports[i]->sprites[0]));
        VIEWPORT* v = _game_viewports[i];
        //printf("[INFO] param v{%p}\n", v);
        game_viewport_draw(_game_viewports[i]);
    }

    // Present
    SDL_RenderPresent(_game_renderer);
}
