#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#include "common.h"

typedef struct main_menu_state_t
{
    u32 current_selection;
    // TODO(@sebbe): data that we need in the main menu ...
} main_menu_state_t;

main_menu_state_t main_menu_state;

void main_menu_update(f32 delta_time);
void main_menu_render();

#endif