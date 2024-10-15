#include "include/common.h"
#include "include/developer.h"

void developer_menu_update()
{

    if (IsKeyPressed(KEY_UP))
    {
        if (developer_menu_state.selection > gs_main_menu)
        {
            developer_menu_state.selection--;
        }
    }
    else if (IsKeyPressed(KEY_DOWN))
    {
        if (developer_menu_state.selection < NUM_GAME_STATES - 1)
        {
            developer_menu_state.selection++;
        }
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        if (developer_menu_state.selection == gs_multiplayer)
        {
            inet_address_t server_ip = "127.0.0.1";
            connection = connect_tcp_server(server_ip);

            connect_udp_server();

            GAME_MODE game_mode = MULTIPLAYER;

            game_state = gs_world_map;
            return;
        }

        game_state = developer_menu_state.selection;
    }
}

void developer_menu_render()
{
    static i32 menu_offset_x = 100;
    static i32 menu_offset_y = 100;
    static i32 menu_font_size = 21;
    static i32 menu_spacing = 32;

    for (u32 i = gs_main_menu; i < NUM_GAME_STATES; i++)
    {
        DrawText(developer_menu_strings[i],
                 menu_offset_x, menu_spacing * i + menu_offset_y, menu_font_size,
                 i == developer_menu_state.selection ? YELLOW : GRAY);
    }
}