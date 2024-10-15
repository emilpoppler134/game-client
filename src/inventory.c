#include "include/common.h"
#include "include/inventory.h"

bool inventory_move_item(byte from, byte to)
{
    if (inventory[to])
    {
        printf("Destination already occupied ...\n");
        return false;
    }
    rect_t target = {
        .x = (to % INVENTORY_WIDTH) * INVENTORY_GRID,
        .y = (to / INVENTORY_WIDTH) * INVENTORY_GRID,
        .width = cargo_db[inventory[from] - 1].width * INVENTORY_GRID,
        .height = cargo_db[inventory[from] - 1].height * INVENTORY_GRID};
    for (byte i = 0; i < INVENTORY_WIDTH * INVENTORY_HEIGHT; i++)
    {
        if (inventory[i])
        {
            rect_t test = {
                .x = (i % INVENTORY_WIDTH) * INVENTORY_GRID,
                .y = (i / INVENTORY_WIDTH) * INVENTORY_GRID,
                .width = cargo_db[inventory[i] - 1].width * INVENTORY_GRID,
                .height = cargo_db[inventory[i] - 1].height * INVENTORY_GRID};

            if (CheckCollisionRecs(target, test))
            {
                return false;
            }
        }
    }
    inventory[to] = inventory[from];
    inventory[from] = 0;
    inventory_state.selected_tile = -1;
    return true;
}

void inventory_update(f32 delta_time)
{
    if (IsKeyPressed(KEY_BACKSPACE))
    {
        game_state = gs_developer_menu;
    }

    rect_t grid_rect = {
        .x = GAME_WIDTH / 2 - ((INVENTORY_GRID * INVENTORY_WIDTH) / 2),
        .y = 16,
        .width = INVENTORY_GRID * INVENTORY_WIDTH,
        .height = INVENTORY_GRID * INVENTORY_HEIGHT};

    vec2_t mouse_pos = GetMousePosition();

    inventory_state.mouse_tile = -1;
    if (CheckCollisionPointRec(mouse_pos, grid_rect))
    {
        i32 mouse_tile_x = (i32)(mouse_pos.x - grid_rect.x) / INVENTORY_GRID;
        i32 mouse_tile_y = (i32)(mouse_pos.y - grid_rect.y) / INVENTORY_GRID;
        inventory_state.mouse_tile = mouse_tile_y * INVENTORY_WIDTH + mouse_tile_x;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (inventory[inventory_state.mouse_tile])
            {
                inventory_state.selected_tile = inventory_state.mouse_tile;
            }
            else
            {
                if (inventory_state.selected_tile > -1)
                {
                    inventory_move_item(inventory_state.selected_tile, inventory_state.mouse_tile);
                }
            }
        }
    }
}

void grid_draw(u32 x_offset, u32 y_offset, u32 x_num, u32 y_num, u32 size, color_t color)
{
    for (u32 i = 0; i < y_num + 1; i++)
    {
        DrawLine(x_offset, y_offset + i * size,
                 x_offset + (size * x_num), y_offset + i * size, color);
    }
    for (u32 i = 0; i < x_num + 1; i++)
    {
        DrawLine(x_offset + i * size, y_offset,
                 x_offset + i * size, y_offset + y_num * size,
                 color);
    }
}

void inventory_render()
{
    u32 grid_pos_x = GAME_WIDTH / 2 - ((INVENTORY_GRID * INVENTORY_WIDTH) / 2);
    u32 grid_pos_y = 16;
    grid_draw(grid_pos_x, grid_pos_y, INVENTORY_WIDTH, INVENTORY_HEIGHT, INVENTORY_GRID, GRAY);

    for (byte i = 0; i < INVENTORY_WIDTH * INVENTORY_HEIGHT; i++)
    {
        if (inventory[i])
        {
            rect_t item_rect = {
                .x = (i % INVENTORY_WIDTH) * INVENTORY_GRID + grid_pos_x,
                .y = (i / INVENTORY_WIDTH) * INVENTORY_GRID + grid_pos_y,
                .width = cargo_db[inventory[i] - 1].width * INVENTORY_GRID,
                .height = cargo_db[inventory[i] - 1].height * INVENTORY_GRID};
            DrawRectangleRec(item_rect, inventory_state.selected_tile == i ? YELLOW : inventory_state.mouse_tile == i ? GREEN
                                                                                                                      : RED);
            DrawText(cargo_db[inventory[i] - 1].name, item_rect.x + 2, item_rect.y + 2, 8, WHITE);
        }
    }

    DrawText(TextFormat("Mouse Tile: %d", inventory_state.mouse_tile), 8, 8, 16, WHITE);
}