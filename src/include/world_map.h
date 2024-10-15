#ifndef __WORLD_MAP_H__
#define __WORLD_MAP_H__

#include "common.h"

// STRUCTS :
typedef struct world_map_state_t
{
    byte current_chunks;
    // TODO(@sebbe) : All of the stuff we need for the world map goes here ...
} world_map_state_t;

// NOTE(@sebbe) : is probably not going to be global in the end
// but it is by far the easiet way to deal with it for now,
// so try to resist the temptantion of making this malloc'd or
// something local that we pass around ...
world_map_state_t world_map_state;

typedef struct loaded_chunks_t
{
    byte nw[CHUNK_TILE_COUNT * CHUNK_TILE_COUNT];
    byte ne[CHUNK_TILE_COUNT * CHUNK_TILE_COUNT];
    byte sw[CHUNK_TILE_COUNT * CHUNK_TILE_COUNT];
    byte se[CHUNK_TILE_COUNT * CHUNK_TILE_COUNT];
} loaded_chunks_t;

// PROTOTYPES :
void world_map_setup();
void world_map_update(f32 delta_time);
void world_map_render();

#endif