#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "common.h"

typedef enum settings_type_e
{
    settings_type_fullscreen,
    settings_type_resolution,
    settings_type_volume,
} settings_type_e;

typedef struct settings_rec_t
{
    settings_type_e type;

    f32 current_val;

    f32 max_val;
    f32 min_val;

    /*
    MAX 35 CHAR
    */
    byte hover_text[35];
    /*
    MAX 15 CHAR
    */
    byte text[15];
    /*
    Resolution goes from how large every step is in the slider in the settings page.
    If you have resolution of 20 on a 0 to 100 slider you get 5 steps of 20 units each.
    */
    u16 resolution;
} settings_rec_t;

typedef struct settings_s
{
    u32 window_width;
    u32 window_height;
    f32 volume_all;
    f32 volume_waves;
    f32 volume_effects;
    f32 volume_inventory;
    bool fullscreen;
} settings_s;

settings_s read_settings();
void apply_settings(settings_s *);
void settings_page_render();

#endif