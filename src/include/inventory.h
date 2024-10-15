#ifndef __INVENTORY_H__
#define __INVENTORY_H__

#include "common.h"

#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 8
#define INVENTORY_GRID 32

typedef struct cargo_info_t
{
    char *name;
    byte width;
    byte height;
    u32 weight;
} cargo_info_t;

cargo_info_t cargo_db[256] = {
    {.name = "Grain",
     .width = 1,
     .height = 1,
     .weight = 100},
    {.name = "Livestock",
     .width = 2,
     .height = 2,
     .weight = 200},
    {.name = "Gold",
     .width = 1,
     .height = 1,
     .weight = 1000},
};

typedef struct inventory_state_t
{
    i32 mouse_tile;
    i32 selected_tile;
} inventory_state_t;

byte inventory[INVENTORY_WIDTH * INVENTORY_HEIGHT];
inventory_state_t inventory_state = {
    .mouse_tile = -1};

void inventory_update(f32 delta_time);
void inventory_render();

#endif