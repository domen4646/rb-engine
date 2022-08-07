#ifndef RUBYUTILS_H
#define RUBYUTILS_H

#include "common.h"

bool extend_ruby_all();

void rubyutil_register_events_module();
void rubyutil_register_graphics_module();
void rubyutil_register_collision_module();
void rubyutil_register_texture_class();
void rubyutil_register_viewport_class();
void rubyutil_register_sprite_class();


VIEWPORT* get_viewport_data_from(VALUE viewport);
TEXTURE_DATA* get_texture_data_from(VALUE texture);
SPRITE* get_sprite_data_from(VALUE sprite);

void game_sprite_draw(SPRITE* sprite);
void game_viewport_draw(VIEWPORT* viewport);

#endif // RUBYUTILS_H
