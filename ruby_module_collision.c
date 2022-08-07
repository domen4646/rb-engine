#include "rubyutils.h"

VALUE _game_collision_module;

VALUE _sprites_intersect(VALUE sprite1, VALUE sprite2);

void rubyutil_register_collision_module() {
    _game_collision_module = rb_define_module("Collision");

}

VALUE _sprites_intersect(VALUE sprite1, VALUE sprite2) {

    if (CLASS_OF(sprite1) != _game_sprite_class || CLASS_OF(sprite2) != _game_sprite_class) {
        rb_raise(rb_eArgError, "Expected a sprite as the argument, got something else instead.");
        return Qfalse;
    }

    SPRITE* s1 = get_sprite_data_from(sprite1);
    SPRITE* s2 = get_sprite_data_from(sprite2);

    ccd_t ccd;
    CCD_INIT(&ccd);

    ccd.support1 = sprite_support;
    ccd.support2 = sprite_support;
    ccd.max_iterations = 100;
    int intersect = ccdGJKIntersect((const void*) s1, (const void*) s2, &ccd);
    return (intersect == true) ? Qtrue : Qfalse;
}
