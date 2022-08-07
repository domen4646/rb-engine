#include "rubyutils.h"

bool extend_ruby_all() {
    rubyutil_register_events_module();
    rubyutil_register_graphics_module();
    rubyutil_register_texture_class();
    rubyutil_register_viewport_class();
    rubyutil_register_sprite_class();
    return true;
}
