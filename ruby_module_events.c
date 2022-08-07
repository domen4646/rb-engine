#include "common.h"
#include "rubyutils.h"

typedef struct _sdl_keystate {
    bool pressed;
} KEYSTATE;

KEYSTATE _keystates[512];
bool _should_quit = false;


// Ruby data
VALUE eventsModule;
VALUE _events_should_quit();
void _process_events();

void rubyutil_register_events_module() {
    eventsModule = rb_define_module("Events");

    rb_define_module_function(eventsModule, "should_quit", _events_should_quit, 0);
    rb_define_module_function(eventsModule, "process", _process_events, 0);
}

void _process_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {

        switch (event.type) {

        case SDL_QUIT:
            _should_quit = true;
            break;

        case SDL_KEYDOWN:
            _keystates[event.key.keysym.scancode].pressed = true;
            break;

        case SDL_KEYUP:
            _keystates[event.key.keysym.scancode].pressed = false;
            break;

        }

    }
}

VALUE _events_should_quit() {
    return _should_quit ? Qtrue : Qfalse;
    //return INT2FIX((int)_should_quit);
}
