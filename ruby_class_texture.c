#include "rubyutils.h"

VALUE _game_texture_class;

void texture_data_free(void* data) {
    TEXTURE_DATA* tdata = (TEXTURE_DATA*) data;
    if (tdata->owns_data && tdata->texture != NULL) {
        SDL_DestroyTexture(tdata->texture);
    }
    if (tdata->error != NULL) {
        free((void*)tdata->error);
    }
    free(data);
}

size_t texture_data_size(const void* data) {
    return sizeof(TEXTURE_DATA);
}

// Class data declaration
static const rb_data_type_t texture_data_type = {
    .wrap_struct_name = "TextureData",
    .function = {
        .dmark = NULL,
        .dfree = texture_data_free,
        .dsize = texture_data_size
    },
    .data = NULL,
    .flags = RUBY_TYPED_FREE_IMMEDIATELY
};

VALUE texture_alloc(VALUE self);
VALUE texture_initialize(VALUE self, VALUE filename);
VALUE texture_valid(VALUE self);
VALUE texture_get_width(VALUE self);
VALUE texture_get_height(VALUE self);
void texture_draw(VALUE self, VALUE x, VALUE y);


void rubyutil_register_texture_class() {
    VALUE texture_class = rb_define_class("Texture", rb_cObject);
    _game_texture_class = texture_class;
    rb_define_alloc_func(texture_class, texture_alloc);
    rb_define_method(texture_class, "initialize", texture_initialize, 1);
    rb_define_method(texture_class, "valid?", texture_valid, 0);
    rb_define_method(texture_class, "draw", texture_draw, 2);
    rb_define_method(texture_class, "width", texture_get_width, 0);
    rb_define_method(texture_class, "height", texture_get_height, 0);
}

VALUE texture_alloc(VALUE self) {
    TEXTURE_DATA* data = (TEXTURE_DATA*) malloc(sizeof(TEXTURE_DATA));
    data->owns_data = false;
    data->texture = NULL;
    data->error = NULL;
    data->width = -1;
    data->height = -1;
    return TypedData_Wrap_Struct(self, &texture_data_type, data);
}

VALUE texture_initialize(VALUE self, VALUE filename) {
    TEXTURE_DATA* data;
    TypedData_Get_Struct(self, TEXTURE_DATA, &texture_data_type, data);

    data->owns_data = true;
    char* str_filename = strndup(StringValuePtr(filename), RSTRING_LEN(filename));
    if (str_filename == NULL) {
        data->error = strdup("[NON-SDL ERROR] The value given could not be converted to string.");
        return self;
    }
    data->texture = IMG_LoadTexture(_game_renderer, (const char*)str_filename);
    if (data->texture == NULL) {
        data->error = strdup(SDL_GetError());
    }
    else {
        SDL_QueryTexture(data->texture, NULL, NULL, &data->width, &data->height);
    }
    return self;
}

VALUE texture_valid(VALUE self) {
    TEXTURE_DATA* data;
    TypedData_Get_Struct(self, TEXTURE_DATA, &texture_data_type, data);
    return (data->texture != NULL) ? Qtrue : Qfalse;
}

void texture_draw(VALUE self, VALUE x, VALUE y) {
    TEXTURE_DATA* data;
    TypedData_Get_Struct(self, TEXTURE_DATA, &texture_data_type, data);
    if (data->texture == NULL)
        return;

    SDL_Rect dst;
    dst.x = FIX2INT(x);
    dst.y = FIX2INT(y);
    dst.w = data->width;
    dst.h = data->height;
    SDL_RenderCopy(_game_renderer, data->texture, NULL, &dst);
}

VALUE texture_get_width(VALUE self) {
    return INT2FIX(get_texture_data_from(self)->width);
}

VALUE texture_get_height(VALUE self) {
    return INT2FIX(get_texture_data_from(self)->height);
}

TEXTURE_DATA* get_texture_data_from(VALUE texture) {
    TEXTURE_DATA* data;
    TypedData_Get_Struct(texture, TEXTURE_DATA, &texture_data_type, data);
    return data;
}
