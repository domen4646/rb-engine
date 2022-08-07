#include "rubyutils.h"

VALUE _game_sprite_class;

size_t _sprite_size(const void* data) {
    return sizeof(SPRITE);
}

void _sprite_free(void* data) {
    printf("Sprite free\n");
    SPRITE* s = (SPRITE*) data;
    VIEWPORT* v = _internal_global_viewport;
    if (s->viewport != Qnil && CLASS_OF(s->viewport) == _game_viewport_class) {
        v = get_sprite_data_from(s->viewport);
    }

    cvector_find_and_remove_all(v->sprites[s->z_index], s->__self);
    free(data);
}

void _sprite_mark(void* data) {
    printf("Sprite mark\n");
    SPRITE* s = (SPRITE*) data;
    rb_gc_mark(s->texture);
}

// Class data declaration
static const rb_data_type_t sprite_data_type = {
    .wrap_struct_name = "SpriteData",
    .function = {
        .dmark = _sprite_mark,
        .dfree = _sprite_free,
        .dsize = _sprite_size
    },
    .data = NULL,
    .flags = RUBY_TYPED_FREE_IMMEDIATELY
};

VALUE _sprite_alloc(VALUE self);
VALUE _sprite_initialize(int argc, VALUE* argv, VALUE self);
VALUE _sprite_shown(VALUE self);
void _sprite_show(VALUE self);
void _sprite_hide(VALUE self);
void _sprite_position(VALUE self, VALUE x, VALUE y);

void rubyutil_register_sprite_class() {
    _game_sprite_class = rb_define_class("Sprite", rb_cObject);

    rb_define_alloc_func(_game_sprite_class, _sprite_alloc);
    rb_define_method(_game_sprite_class, "initialize", _sprite_initialize, -1);
    rb_define_method(_game_sprite_class, "shown?", _sprite_shown, 0);
    rb_define_method(_game_sprite_class, "show", _sprite_show, 0);
    rb_define_method(_game_sprite_class, "hide", _sprite_hide, 0);
    rb_define_method(_game_sprite_class, "position", _sprite_position, 2);
}

VALUE _sprite_alloc(VALUE self) {
    SPRITE* data;
    return TypedData_Make_Struct(self, SPRITE, &sprite_data_type, data);
}

VALUE _sprite_initialize(int argc, VALUE* argv, VALUE self) {
    SPRITE* data;
    TypedData_Get_Struct(self, SPRITE, &sprite_data_type, data);
    data->single_sprite = true;
    data->destination.x = 0;
    data->destination.y = 0;
    data->destination.w = 0;
    data->destination.h = 0;
    data->degrees = 0.0F;
    data->z_index = 0;
    data->ignores_offset = false;
    //data->scale_x = 1.0F;
    //data->scale_y = 1.0F;
    data->shown = true;
    data->viewport = Qnil;
    data->__self = self;

    if (argc == 1) {
        if (CLASS_OF(argv[0]) != _game_viewport_class && argv[0] != Qnil) {
            rb_raise(rb_eArgError, "Argument 1: Excpected viewport, but got something else");
            return self;
        }
        data->viewport = argv[0];
        VIEWPORT* v = (data->viewport == Qnil) ? _internal_global_viewport : get_viewport_data_from(data->viewport);
        cvector_push_back(v->sprites[data->z_index], self);
        return self;
    }

    if (argc == 2 || argc == 4) {

        int x = 0;
        int y = 0;
        if (argc == 4) {
            if (!RB_TYPE_P(argv[2], T_FIXNUM) || !RB_TYPE_P(argv[3], T_FIXNUM)) {
                rb_raise(rb_eArgError, "Arguments 3, 4: Excpected fixnum (number), but got something else");
                return self;
            }
            x = FIX2INT(argv[2]);
            y = FIX2INT(argv[3]);
        }

        if (CLASS_OF(argv[0]) != _game_viewport_class && argv[0] != Qnil) {
            rb_raise(rb_eArgError, "Argument 1: Excpected viewport, but got something else");
            return self;
        }
        data->viewport = argv[0];
        VIEWPORT* v = (data->viewport == Qnil) ? _internal_global_viewport : get_viewport_data_from(data->viewport);
        cvector_push_back(v->sprites[data->z_index], self);
        printf("Adding to viewport %p. the current size is %i [%p]\n", v, cvector_size(v->sprites[data->z_index]), v->sprites[0]);

        VALUE texture = Qnil;
        if (CLASS_OF(argv[1]) == rb_cString) {
            VALUE args[1] = {argv[1]};
            texture = rb_class_new_instance(1, args, _game_texture_class);
        }
        else if (CLASS_OF(argv[1]) == _game_texture_class)
            texture = argv[1];
        else {
            rb_raise(rb_eArgError, "Argument 2: The argument must be a String or a Texture!");
            return self;
        }
        if (CLASS_OF(texture) != _game_texture_class) {
            rb_raise(rb_eArgError, "Failed to initialize a Texture!");
            return self;
        }

        TEXTURE_DATA* texture_data = get_texture_data_from(texture);

        data->texture = texture;
        data->destination.x = x;
        data->destination.y = y;
        data->destination.w = texture_data->width;
        data->destination.h = texture_data->height;
        printf("len of [%p] => %i\n", _game_viewports[0]->sprites[0], cvector_size(_game_viewports[0]->sprites[0]));

        return self;
    }

    rb_raise(rb_eArgError, "Wrong number of arguments!");
    return self;
}



// Methods
VALUE _sprite_shown(VALUE self) {
    SPRITE* data;
    TypedData_Get_Struct(self, SPRITE, &sprite_data_type, data);
    return (data->shown) ? Qtrue : Qfalse;
}

void _sprite_show(VALUE self) {
    SPRITE* data;
    TypedData_Get_Struct(self, SPRITE, &sprite_data_type, data);
    data->shown = true;
}

void _sprite_hide(VALUE self) {
    SPRITE* data;
    TypedData_Get_Struct(self, SPRITE, &sprite_data_type, data);
    data->shown = false;
}

void _sprite_position(VALUE self, VALUE x, VALUE y) {
    Check_Type(x, T_FIXNUM);
    Check_Type(y, T_FIXNUM);

    SPRITE* sprite = get_sprite_data_from(self);
    sprite->destination.x = FIX2INT(x);
    sprite->destination.y = FIX2INT(y);
}

SPRITE* get_sprite_data_from(VALUE sprite) {
    SPRITE* data;
    TypedData_Get_Struct(sprite, SPRITE, &sprite_data_type, data);
    return data;
}

// Draws the sprite on the current viewport
void game_sprite_draw(SPRITE* sprite) {
    if (!sprite->shown || sprite->texture == Qnil) {
        return;
    }
    VIEWPORT* viewport = (sprite->viewport != Qnil) ? get_viewport_data_from(sprite->viewport) : _internal_global_viewport;
    TEXTURE_DATA* texture_data = get_texture_data_from(sprite->texture);
    SDL_RendererFlip flip = 0;
    if (sprite->flip_x)
        flip |= SDL_FLIP_HORIZONTAL;
    if (sprite->flip_y)
        flip |= SDL_FLIP_VERTICAL;

    if (!sprite->ignores_offset || viewport->force_offset) {
        SDL_Rect dest = sprite->destination;
        dest.x += viewport->offset_x;
        dest.y += viewport->offset_y;

        if (sprite->single_sprite) {
            SDL_RenderCopyEx(_game_renderer, texture_data->texture, NULL, &dest, sprite->degrees, NULL, flip);
        }
        else {
            SDL_RenderCopyEx(_game_renderer, texture_data->texture, &sprite->source, &dest, sprite->degrees, NULL, flip);
        }
        return;
    }

    if (sprite->single_sprite) {
        SDL_RenderCopyEx(_game_renderer, texture_data->texture, NULL, &sprite->destination, sprite->degrees, NULL, flip);
    }
    else {
        SDL_RenderCopyEx(_game_renderer, texture_data->texture, &sprite->source, &sprite->destination, sprite->degrees, NULL, flip);
    }
    return;
}
