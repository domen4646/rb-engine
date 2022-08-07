#include "rubyutils.h"

VALUE _game_viewport_class;

size_t _viewport_size(const void* data) {
    return sizeof(VIEWPORT);
}

void _viewport_free(void* data) {
    VIEWPORT* v = (VIEWPORT*) data;
    cvector_find_and_remove_all_viewports(_game_viewports, v);
    for (int i=0; i<256; i++) {
        cvector_free(v->sprites[i]);
    }
    free(data);
}

// Class data declaration
static const rb_data_type_t viewport_data_type = {
    .wrap_struct_name = "ViewportData",
    .function = {
        .dmark = NULL,
        .dfree = _viewport_free,
        .dsize = _viewport_size
    },
    .data = NULL,
    .flags = RUBY_TYPED_FREE_IMMEDIATELY
};

VALUE _viewport_alloc(VALUE self);
VALUE _viewport_initialize(int argc, VALUE* argv, VALUE self);
VALUE _viewport_shown(VALUE self);
void _viewport_show(VALUE self);
void _viewport_hide(VALUE self);

void rubyutil_register_viewport_class() {
    _game_viewport_class = rb_define_class("Viewport", rb_cObject);

    rb_define_alloc_func(_game_viewport_class, _viewport_alloc);
    rb_define_method(_game_viewport_class, "initialize", _viewport_initialize, -1);
    rb_define_method(_game_viewport_class, "shown?", _viewport_shown, 0);
    rb_define_method(_game_viewport_class, "show", _viewport_show, 0);
    rb_define_method(_game_viewport_class, "hide", _viewport_hide, 0);
}

VALUE _viewport_alloc(VALUE self) {
    VIEWPORT* data = (VIEWPORT*) malloc(sizeof(VIEWPORT));
    for (int i=0; i<256; i++) {
        data->sprites[i] = NULL;
    }
    return TypedData_Wrap_Struct(self, &viewport_data_type, data);
}

VALUE _viewport_initialize(int argc, VALUE* argv, VALUE self) {
    VIEWPORT* data;
    TypedData_Get_Struct(self, VIEWPORT, &viewport_data_type, data);
    data->force_offset = false;
    data->offset_x = 0;
    data->offset_y = 0;
    data->scale_x = 1.0F;
    data->scale_y = 1.0F;
    data->shown = true;
    if (argc == 0) {
        data->viewport_rect.x = 0;
        data->viewport_rect.y = 0;
        data->viewport_rect.w = get_window_width();
        data->viewport_rect.h = get_window_height();
        cvector_push_back(_game_viewports, data);
        return self;
    }

    if (argc == 2) {
        Check_Type(argv[0], T_FIXNUM);
        Check_Type(argv[1], T_FIXNUM);
        data->viewport_rect.x = 0;
        data->viewport_rect.y = 0;
        data->viewport_rect.w = FIX2INT(argv[0]);
        data->viewport_rect.h = FIX2INT(argv[1]);
        cvector_push_back(_game_viewports, data);
        return self;
    }

    if (argc == 4) {
        Check_Type(argv[0], T_FIXNUM);
        Check_Type(argv[1], T_FIXNUM);
        Check_Type(argv[2], T_FIXNUM);
        Check_Type(argv[3], T_FIXNUM);
        data->viewport_rect.x = FIX2INT(argv[0]);
        data->viewport_rect.y = FIX2INT(argv[1]);
        data->viewport_rect.w = FIX2INT(argv[2]);
        data->viewport_rect.h = FIX2INT(argv[3]);
        cvector_push_back(_game_viewports, data);
        return self;
    }
    rb_raise(rb_eArgError, "Wrong number of arguments!");
    return self;
}



// Methods
VALUE _viewport_shown(VALUE self) {
    VIEWPORT* data;
    TypedData_Get_Struct(self, VIEWPORT, &viewport_data_type, data);
    return (data->shown) ? Qtrue : Qfalse;
}

void _viewport_show(VALUE self) {
    VIEWPORT* data;
    TypedData_Get_Struct(self, VIEWPORT, &viewport_data_type, data);
    data->shown = true;
}

void _viewport_hide(VALUE self) {
    VIEWPORT* data;
    TypedData_Get_Struct(self, VIEWPORT, &viewport_data_type, data);
    data->shown = false;
}

// Global function implementation
VIEWPORT* get_viewport_data_from(VALUE viewport) {
    VIEWPORT* data;
    TypedData_Get_Struct(viewport, VIEWPORT, &viewport_data_type, data);
    return data;
}

void game_viewport_draw(VIEWPORT* viewport) {
    if (!viewport->shown) {
        return;
    }
    //printf("[WARN] len of {%p} [%p] => %i\n", viewport, viewport->sprites[0], cvector_size(viewport->sprites[0]));

    SDL_RenderSetViewport(_game_renderer, &viewport->viewport_rect);
    //printf("[%p] size for z=%i => %i\n", viewport->sprites[0], 0, cvector_size(viewport->sprites[0]));
    for (int i=0; i<256; i++) {
        for (int j=0; j<cvector_size(viewport->sprites[i]); j++) {
            game_sprite_draw(get_sprite_data_from(viewport->sprites[i][j]));
        }
    }
}
