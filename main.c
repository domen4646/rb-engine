#include "common.h"
#include "rubyutils.h"

SDL_Window* _game_window = NULL;
SDL_Renderer* _game_renderer = NULL;
cvector_vector_type(VIEWPORT*) _game_viewports = NULL;
VIEWPORT* _internal_global_viewport = NULL;


void _printexception(VALUE exception) {
    VALUE last_exception = rb_funcall(exception, rb_intern("full_message"), 0);
    char* ex = StringValuePtr(last_exception);
    int len = RSTRING_LEN(last_exception);

    fwrite((const void*) ex, 1, len, stdout);
    printf("\n");
}

bool process_rb_file(char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Failed to open: %s.\n", filename);
        return false;
    }
    fseek(fp, 0, SEEK_END);
    size_t sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* data = (char*) malloc(sz + 1);
    size_t nread = fread((void*) data, 1, sz, fp);
    if (nread != sz) {
        fclose(fp);
        printf("Failed to read: %s.\n", filename);
        return false;
    }
    fclose(fp);
    data[sz] = '\0';

    int state;
    VALUE result = rb_eval_string_protect((const char*) data, &state);

    if (state) {
        printf("%s: An exception occured\n", filename);
        VALUE last_exception = rb_errinfo();
        if (last_exception == Qnil) {
            printf("But the exception was nil!\n");
        }
        else {
            _printexception(last_exception);
        }

        rb_set_errinfo(Qnil);
    }

    return true;
}



int main()
{   
    printf("Hello World!\n");
    printf("SDL Init\n");
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        _serr("SDL_Init");

    //if (IMG_Init(0) != 0)
    //    _serr("IMG_Init");

    _game_window = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, get_window_width(), get_window_height(), 0);

    if (!_game_window)
        _serr("SDL_CreateWindow");

    _game_renderer = SDL_CreateRenderer(_game_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!_game_renderer)
        _serr("SDL_CreateRenderer");

    _internal_global_viewport = (VIEWPORT*) malloc(sizeof(VIEWPORT));
    _internal_global_viewport->force_offset = false;
    _internal_global_viewport->offset_x = 0;
    _internal_global_viewport->offset_y = 0;
    _internal_global_viewport->scale_x = 1.0F;
    _internal_global_viewport->scale_y = 1.0F;
    _internal_global_viewport->shown = true;
    _internal_global_viewport->viewport_rect.x = 0;
    _internal_global_viewport->viewport_rect.y = 0;
    _internal_global_viewport->viewport_rect.w = get_window_width();
    _internal_global_viewport->viewport_rect.h = get_window_height();
    for (int i=0; i<256; i++) {
        _internal_global_viewport->sprites[i] = NULL;
    }

    // Ruby
    ruby_init();
    if (!extend_ruby_all()) {
        ruby_cleanup(0);
        printf("Extending of ruby failed!\n");
        return 1;
    }
    printf("Ruby extending successful.\n");

    util_rb_filelist(process_rb_file);


    int state;
    VALUE result = rb_eval_string_protect("game_entry", &state);

    if (state) {
        printf("A runtime exception occured\n");
        VALUE last_exception = rb_errinfo();
        if (last_exception == Qnil) {
            printf("But the exception was nil!\n");
        }
        else {
            _printexception(last_exception);
        }

        rb_set_errinfo(Qnil);
    }


    ruby_cleanup(0);
    SDL_DestroyRenderer(_game_renderer);
    SDL_DestroyWindow(_game_window);

    return 0;
}
