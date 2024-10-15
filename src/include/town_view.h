#ifndef __TOWN_VIEW_H__
#define __TOWN_VIEW_H__

#include "common.h"

typedef enum town_sub_states_e
{
    ts_main = -1,
    ts_tavern,
    ts_shipyard,
    ts_merchant,
    NUM_TOWN_SUB_STATES
} town_sub_states_e;

typedef struct town_selection_state_t
{
    u32 selection;
} town_selection_state_t;

town_selection_state_t town_selection_state = {
    .selection = ts_tavern};

char *town_selection_strings[] = {
    "Go to the tavern",
    "Go to the shipyard",
    "Visit the merchant"};

town_sub_states_e town_sub_states = ts_main;

typedef struct city_t
{
    char *name;
    char *faction;
    u32 population;
    u16 hostility_level;
} city_t;

void text_boxed_draw(const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint);
void text_boxed_selectable_draw(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint);

#endif