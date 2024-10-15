#ifndef __DEVELOPER_H__
#define __DEVELOPER_H__

// STRUCTS :
typedef struct developer_menu_state_t
{
    u32 selection;
} developer_menu_state_t;

// DATA :
developer_menu_state_t developer_menu_state = {
    .selection = gs_main_menu};

char *developer_menu_strings[] = {
    "Developer Menu",
    "Main menu",
    "World map",
    "Multiplayer",
    "Inventory screen",
    "Town view",
    "Game Over",
    "Settings",
};

// PROTOTYPES :
void developer_menu_update();
void developer_menu_render();

#endif
