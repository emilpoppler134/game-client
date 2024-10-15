#include "include/common.h"
#include "include/world_map.h"

loaded_chunks_t loaded_chunk;

Texture2D tilemap;
Music tutorial;

Camera2D camera = {
    {GAME_WIDTH / 2, GAME_HEIGHT / 2},
    {0, 0},
    0,
    1,
};

vec2_t player_position_in_chunk = {400, 400};
byte player_chunk_id = 0;
byte player_offset_chunk_id;
byte player_offset_chunk_id_prev;
vec2_t player_absolute_position;
f32 player_velocity;
f32 player_max_velocity = 250;
f32 player_acceleration = 50;
f32 player_rotation = 0;
f32 player_velocity_loss_from_turning = 0.02;
f32 player_rudder_angle = 0;
f32 player_rudder_turn_speed = 90;
f32 player_rudder_max_angle = 45;
f32 player_rudder_min_angle = -45;

void cache_files()
{
    for (u32 i = 0; i < CHUNK_COUNT * CHUNK_COUNT; i++)
    {
        printf("Cached file: %d\n", i);
        const char *path = TextFormat("src/assets/map/chunks/%03d.chunk", i);
        FILE *file_ptr = fopen(path, "r");
        fclose(file_ptr);
    }    
}

void render_chunk(byte chunkId, byte current_chunk_tiles[]) {
    for (u16 i = 0; i < CHUNK_TILE_COUNT * CHUNK_TILE_COUNT; i++)
    {
        byte n = current_chunk_tiles[i];

        u16 x = i % CHUNK_TILE_COUNT * TILE_SIZE + chunkId % 16 * CHUNK_SIZE;
        u16 y = i / CHUNK_TILE_COUNT * TILE_SIZE + chunkId / 16 * CHUNK_SIZE;

        if (n != 16)
        {
            DrawTexturePro(
                tilemap,
                (Rectangle){n * TEXTURE_TILE_SIZE, 0, TEXTURE_TILE_SIZE, TEXTURE_TILE_SIZE},
                (Rectangle){x, y, TILE_SIZE, TILE_SIZE},
                (Vector2){0, 0},
                0,
                WHITE
            );
        }
    }
}

void load_chunk(byte chunkId, byte loaded_target[])
{
    double start = GetTime();
    
    const char *path = TextFormat("src/assets/map/chunks/%03d.chunk", chunkId);
    FILE *file_ptr = fopen(path, "r");

    double post_fopen = GetTime();

    for (u16 i = 0; i < CHUNK_TILE_COUNT * CHUNK_TILE_COUNT; i += 4)
    {
        loaded_target[i] = fgetc(file_ptr);
        loaded_target[i + 1] = fgetc(file_ptr);
        loaded_target[i + 2] = fgetc(file_ptr);
        loaded_target[i + 3] = fgetc(file_ptr);
    }

    double post_fgetc = GetTime();

    fclose(file_ptr);

    double post_fclose = GetTime();

    puts("Load time:");
    printf("fopen  : %lf ms\n", (post_fopen - start) * 1000);
    printf("fgetc  : %lf ms\n", (post_fgetc - post_fopen) * 1000);
    printf("fclose : %lf ms\n", (post_fclose - post_fgetc) * 1000);
    printf("total  : %lf ms\n", (post_fclose - start) * 1000);
}

void world_map_setup()
{
    tilemap = LoadTexture("src/assets/map/tile_set.png");
    tutorial = LoadMusicStream("src/assets/sounds/tutorial.wav");
    PlayMusicStream(tutorial);
    cache_files();
    load_chunk(player_chunk_id, loaded_chunk.nw);
    load_chunk(player_chunk_id + 1, loaded_chunk.ne);
    load_chunk(player_chunk_id + CHUNK_COUNT, loaded_chunk.sw);
    load_chunk(player_chunk_id + CHUNK_COUNT + 1, loaded_chunk.se);
}

void world_map_update(f32 delta_time)
{
    UpdateMusicStream(tutorial);
    
    if (IsKeyPressed(KEY_BACKSPACE))
    {
        game_state = gs_developer_menu;
    }

    player_offset_chunk_id_prev = player_offset_chunk_id;

    //Boat movement, hate if you can do it better
    if (IsKeyDown(KEY_RIGHT))  player_rudder_angle -= player_rudder_turn_speed * delta_time; 
    if (IsKeyDown(KEY_LEFT)) player_rudder_angle += player_rudder_turn_speed * delta_time; 
    if (IsKeyDown(KEY_LEFT) && IsKeyDown(KEY_RIGHT))
    {
        player_rudder_angle += player_rudder_turn_speed * delta_time * (player_rudder_angle < 0 ? 1 : -1);
    }
    if (IsKeyDown(KEY_UP))
    {
        player_velocity += player_acceleration * delta_time;
    }
    else
    {
        player_velocity -= player_acceleration * delta_time;
    }
    player_rudder_angle = Clamp(player_rudder_angle, player_rudder_min_angle, player_rudder_max_angle);
    player_velocity = player_velocity * (1 - player_velocity_loss_from_turning * fabs(player_rudder_angle) * delta_time);
    player_velocity = Clamp(player_velocity, 0, player_max_velocity);

    // Rotate ship according to rudder_angle TODO and speed
    player_rotation += player_rudder_angle * player_velocity * 0.01 * delta_time;

    player_position_in_chunk = Vector2Add(
        player_position_in_chunk,
        Vector2Rotate((vec2_t){0, -player_velocity * delta_time}, DEG2RAD * -player_rotation)
    ); 

    //vv After all position changing is done vv

    //Wrap player_in_chunk_position when crossing the edge
    if (player_position_in_chunk.x >= CHUNK_SIZE)
    {
        player_chunk_id++;
        if (player_chunk_id % CHUNK_COUNT == 0) player_chunk_id -= CHUNK_COUNT;
        player_position_in_chunk.x -= CHUNK_SIZE;
    }
    if (player_position_in_chunk.x < 0)
    {
        player_chunk_id--;
        if (player_chunk_id % CHUNK_COUNT == CHUNK_COUNT - 1) player_chunk_id += CHUNK_COUNT;
        player_position_in_chunk.x += CHUNK_SIZE;
    }
    if (player_position_in_chunk.y >= CHUNK_SIZE)
    {
        player_chunk_id += CHUNK_COUNT;
        player_position_in_chunk.y -= CHUNK_SIZE;
    }
    if (player_position_in_chunk.y < 0)
    {
        player_chunk_id -= CHUNK_COUNT;
        player_position_in_chunk.y += CHUNK_SIZE;
    }

    player_offset_chunk_id = player_chunk_id;
    if (player_position_in_chunk.x < CHUNK_SIZE / 2) player_offset_chunk_id -= 1;
    if (player_position_in_chunk.y < CHUNK_SIZE / 2) player_offset_chunk_id -= CHUNK_COUNT;

    if (player_offset_chunk_id_prev != player_offset_chunk_id)
    {
        if (player_offset_chunk_id == player_offset_chunk_id_prev + 1)
        {
            puts("====Moved east");
            memcpy(loaded_chunk.nw, loaded_chunk.ne, CHUNK_TILE_COUNT * CHUNK_TILE_COUNT);
            memcpy(loaded_chunk.sw, loaded_chunk.se, CHUNK_TILE_COUNT * CHUNK_TILE_COUNT);
            load_chunk(player_offset_chunk_id + 1, loaded_chunk.ne);
            load_chunk(player_offset_chunk_id + CHUNK_COUNT + 1, loaded_chunk.se);
        }
        else if (player_offset_chunk_id == player_offset_chunk_id_prev - 1)
        {
            puts("====Moved west");
            memcpy(loaded_chunk.ne, loaded_chunk.nw, CHUNK_TILE_COUNT * CHUNK_TILE_COUNT);
            memcpy(loaded_chunk.se, loaded_chunk.sw, CHUNK_TILE_COUNT * CHUNK_TILE_COUNT);
            load_chunk(player_offset_chunk_id, loaded_chunk.nw);
            load_chunk(player_offset_chunk_id + CHUNK_COUNT, loaded_chunk.sw);
        }
        else if (player_offset_chunk_id == player_offset_chunk_id_prev + CHUNK_COUNT)
        {
            puts("====Moved south");
            memcpy(loaded_chunk.nw, loaded_chunk.sw, CHUNK_TILE_COUNT * CHUNK_TILE_COUNT);
            memcpy(loaded_chunk.ne, loaded_chunk.se, CHUNK_TILE_COUNT * CHUNK_TILE_COUNT);
            load_chunk(player_offset_chunk_id + CHUNK_COUNT, loaded_chunk.sw);
            load_chunk(player_offset_chunk_id + CHUNK_COUNT + 1, loaded_chunk.se);
        }
        else if (player_offset_chunk_id == player_offset_chunk_id_prev - CHUNK_COUNT)
        {
            puts("====Moved north");
            memcpy(loaded_chunk.sw, loaded_chunk.nw, CHUNK_TILE_COUNT * CHUNK_TILE_COUNT);
            memcpy(loaded_chunk.se, loaded_chunk.ne, CHUNK_TILE_COUNT * CHUNK_TILE_COUNT);
            load_chunk(player_offset_chunk_id, loaded_chunk.nw);
            load_chunk(player_offset_chunk_id + 1, loaded_chunk.ne);
        }
        else
        {
            puts("====Moved");
            load_chunk(player_offset_chunk_id, loaded_chunk.nw);
            load_chunk(player_offset_chunk_id + 1, loaded_chunk.ne);
            load_chunk(player_offset_chunk_id + CHUNK_COUNT, loaded_chunk.sw);
            load_chunk(player_offset_chunk_id + CHUNK_COUNT + 1, loaded_chunk.se);
        }
    }

    player_absolute_position.x = player_position_in_chunk.x + ((player_chunk_id % CHUNK_COUNT) * CHUNK_SIZE);
    player_absolute_position.y = player_position_in_chunk.y + ((player_chunk_id / CHUNK_COUNT) * CHUNK_SIZE);

    camera.target = player_absolute_position;
    //camera.rotation = player_rotation;

    udp_request_package_t request = {0};
    request.id = connection.id;
    request.player.position = player_absolute_position;
    
    size_t size = sizeof(udp_request_package_t);
    char buffer[size];

    memcpy(&buffer, &request, size);
    send(connection.udp_socket, &buffer, sizeof(buffer), 0);
}

void world_map_render()
{
    ClearBackground(DARKBLUE);

    BeginMode2D(camera);
        render_chunk(player_offset_chunk_id, loaded_chunk.nw);
        render_chunk(player_offset_chunk_id + 1, loaded_chunk.ne);
        render_chunk(player_offset_chunk_id + CHUNK_COUNT, loaded_chunk.sw);
        render_chunk(player_offset_chunk_id + CHUNK_COUNT + 1, loaded_chunk.se);

        for (byte x = 0; x <= CHUNK_COUNT; x++)
        {
            DrawLine(x * CHUNK_SIZE, 0, x * CHUNK_SIZE, CHUNK_COUNT * CHUNK_SIZE, WHITE);
        }
        
        for (byte y = 0; y <= CHUNK_COUNT; y++)
        {
            DrawLine(0, y * CHUNK_SIZE, CHUNK_COUNT * CHUNK_SIZE, y * CHUNK_SIZE, WHITE);
        }
        
        DrawRectanglePro((Rectangle){player_absolute_position.x, player_absolute_position.y, 40, 80}, (Vector2){20, 40}, -player_rotation, RED);

        if (game_mode == MULTIPLAYER)
        {
            for (int i = 0; i < MAX_CLIENTS - 1; i++)
            {
                DrawRectanglePro((Rectangle){opponents[i].position.x, opponents[i].position.y, 40, 80}, (Vector2){20, 40}, 0, BLUE);
            }
        }
    EndMode2D();

    /* DrawTriangle(
        (Vector2){GAME_WIDTH/2 - 20,GAME_HEIGHT/2 + 40},
        (Vector2){GAME_WIDTH/2 + 20,GAME_HEIGHT/2 + 40},
        (Vector2){GAME_WIDTH/2, GAME_HEIGHT/2 - 40},
        BROWN
    );
    DrawLineV(
        (vec2_t){GAME_WIDTH/2, GAME_HEIGHT/2 + 40},
        Vector2Add((vec2_t){GAME_WIDTH/2, GAME_HEIGHT/2 + 40},Vector2Rotate((vec2_t){0,10}, DEG2RAD * player_rudder_angle)),
        BROWN
    ); */
    
}