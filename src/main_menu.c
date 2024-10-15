#include "include/common.h"
#include "include/main_menu.h"

void main_menu_update(f32 delta_time)
{
    // NOTE(@sebbe) : in the developer version, each state needs a way to get back to the developer menu
    if (IsKeyPressed(KEY_BACKSPACE))
    {
        game_state = gs_developer_menu;
    }

    // TODO(@sebbe): do a lot of important main menu logic here
}

void main_menu_render()
{
    DrawText("Man-o'-War", (GAME_WIDTH / 2) - (MeasureText("Man Of War", GAME_HEIGHT / 5) / 2), GAME_HEIGHT / 5, GAME_HEIGHT / 5, WHITE);
    const char *version_text = TextFormat("Version %d.%d", VERSION_MAJOR, VERSION_MINOR);
    DrawText(version_text, GAME_WIDTH - MeasureText(version_text, 16) - 16, 16, 16, WHITE);

    // TODO(@sebbe): render a real main menu here
}