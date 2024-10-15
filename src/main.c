#include "include/common.h"
#include "include/server.h"

#include "include/developer.h"
#include "include/main_menu.h"
#include "include/world_map.h"
#include "include/inventory.h"
#include "include/settings.h"
#include "include/town_view.h"
#include "include/tavern.h"
#include "include/shipyard.h"
#include "include/merchant.h"
#include "include/udp.h"

#include "developer.c"
#include "main_menu.c"
#include "world_map.c"
#include "inventory.c"
#include "tavern.c"
#include "shipyard.c"
#include "merchant.c"
#include "town_view.c"
#include "settings.c"
#include "server.c"
#include "udp.c"

void update(f32 delta_time)
{
    switch (game_state) // defined in common.h
    {
    case gs_developer_menu:
    {
        developer_menu_update();
    }
    break;
    case gs_main_menu:
    {
        main_menu_update(delta_time);
    }
    break;
    case gs_world_map:
    {
        world_map_update(delta_time);
    }
    break;
    case gs_inventory:
    {
        inventory_update(delta_time);
    }
    break;
    case gs_town_view:
    {
        town_view_update(delta_time);
    }
    break;
    case gs_game_over:
    {
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            game_state = gs_developer_menu;
        }
    }
    break;
    default:
        break;
    }
}

RenderTexture2D canvas;

void render()
{

    BeginDrawing();

    BeginTextureMode(canvas);
    ClearBackground(BLACK);
    switch (game_state) // defined in common.h
    {
    case gs_developer_menu:
    {
        developer_menu_render();
    }
    break;
    case gs_main_menu:
    {
        main_menu_render();
    }
    break;
    case gs_world_map:
    {
        world_map_render();
    }
    break;
    case gs_inventory:
    {
        inventory_render();
    }
    break;
    case gs_town_view:
    {
        town_view_render();
    }
    break;
    case gs_game_over:
    {
    }
    break;
    case gs_settings:
    {
        settings_page_render();
    }
    break;
    default:
        break;
    }

    EndTextureMode();

    DrawTexturePro(canvas.texture,
                   (rect_t){0, 0, GAME_WIDTH, -GAME_HEIGHT},
                   (rect_t){0, 0, GetRenderWidth(), GetRenderHeight()},
                   (vec2_t){0.0f, 0.0f},
                   0.0f,
                   WHITE);

    vec2_t virtual_mouse = get_virtual_mouse();
    DrawText(TextFormat("x: %f \t y: %f", virtual_mouse.x, virtual_mouse.y), 8, 8, 16, WHITE);

    EndDrawing();
}

int main()
{
    printf("Reading settings!\n");
    settings_s settings = read_settings();
    InitWindow(settings.window_width, settings.window_height, TextFormat("Man Of War v%d.%d", VERSION_MAJOR, VERSION_MINOR));

    InitAudioDevice();

    canvas = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);

    if (settings.fullscreen)
        ToggleFullscreen();

    world_map_setup();

    inventory[0] = 1;
    inventory[5] = 2;
    inventory[30] = 3;

    while (!WindowShouldClose())
    {
        update(GetFrameTime());
        render();
    }
    return 0;
}
