#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"
#include "server.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 1

#define GAME_WIDTH 1920 / 2
#define GAME_HEIGHT 1080 / 2

#define PIXEL_SCALE 2 // a pixel in a texture will be drawn at 2x scale
#define TILE_SIZE 32  // pixels wide and tall
#define TEXTURE_TILE_SIZE (TILE_SIZE / PIXEL_SCALE)
#define CHUNK_TILE_COUNT (MAP_SIZE / CHUNK_COUNT)
#define CHUNK_SIZE (TILE_SIZE * CHUNK_TILE_COUNT)
#define CHUNK_COUNT 16 // 16x16=256
#define MAP_SIZE 256

typedef uint8_t byte;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef Vector2 vec2_t;
typedef Rectangle rect_t;
typedef Color color_t;

// STRUCTS :
typedef struct point_t
{
    i32 x;
    i32 y;
} point_t;

// ENUMS :
typedef enum game_state_e
{
    gs_developer_menu,
    gs_main_menu,
    gs_world_map,
    gs_multiplayer,
    gs_inventory,
    gs_town_view,
    gs_game_over,
    gs_settings,
    NUM_GAME_STATES
} game_state_e;

// DATA :

game_state_e game_state = gs_developer_menu;

connection_t connection = {0};
player_t opponents[MAX_CLIENTS - 1] = {0};

typedef enum GAME_MODE
{
    SINGLEPLAYER,
    MULTIPLAYER
} GAME_MODE;

GAME_MODE game_mode = SINGLEPLAYER;

vec2_t get_virtual_mouse()
{
    vec2_t mouse = GetMousePosition();
    mouse.x *= (f32)GAME_WIDTH / GetRenderWidth();
    mouse.y *= (f32)GAME_HEIGHT / GetRenderHeight();
    return mouse;
}

#endif
