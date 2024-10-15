#include "include/common.h"
#include "include/town_view.h"
#include "include/tavern.h"

// TODO(@Paul): Make the text box size dynamic based on text length

void town_view_update(f32 delta_time)
{
    if (IsKeyPressed(KEY_BACKSPACE))
    {
        game_state = gs_developer_menu;
    }

    switch (town_sub_states)
    {
    case ts_main:
    {
        if (IsKeyPressed(KEY_UP))
        {
            if (town_selection_state.selection > 0)
            {
                town_selection_state.selection--;
            }
        }
        else if (IsKeyPressed(KEY_DOWN))
        {
            if (town_selection_state.selection < NUM_TOWN_SUB_STATES - 1)
            {
                town_selection_state.selection++;
            }
        }

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        {
            town_sub_states = town_selection_state.selection;
        }
    }
    break;
    case ts_tavern:
    {
        tavern_update(delta_time);
    }
    break;
    case ts_shipyard:
    {
        shipyard_update(delta_time);
    }
    break;
    case ts_merchant:
    {
        merchant_update(delta_time);
    }
    break;
    }
}

void text_boxed_draw(const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint)
{
    text_boxed_selectable_draw(GetFontDefault(), text, rec, fontSize, spacing, wordWrap, tint, 0, 0, WHITE, WHITE);
}

// Draw text using font inside rectangle limits with support for text selection
void text_boxed_selectable_draw(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint)
{
    i32 length = TextLength(text); // Total length in bytes of the text, scanned by codepoints in loop

    f32 textOffsetY = 0;    // Offset between lines (on line break '\n')
    f32 textOffsetX = 0.0f; // Offset X to next character to draw

    f32 scaleFactor = fontSize / (float)font.baseSize; // Character rectangle scaling factor

    // Word/character wrapping mechanism variables
    enum
    {
        MEASURE_STATE = 0,
        DRAW_STATE = 1
    };
    i8 state = wordWrap ? MEASURE_STATE : DRAW_STATE;

    i16 startLine = -1; // Index where to begin drawing (where a line begins)
    i16 endLine = -1;   // Index where to stop drawing (where a line ends)
    i16 lastk = -1;     // Holds last value of the character position

    for (i32 i = 0, k = 0; i < length; i++, k++)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f)
            codepointByteCount = 1;
        i += (codepointByteCount - 1);

        float glyphWidth = 0;
        if (codepoint != '\n')
        {
            glyphWidth = (font.glyphs[index].advanceX == 0) ? font.recs[index].width * scaleFactor : font.glyphs[index].advanceX * scaleFactor;

            if (i + 1 < length)
                glyphWidth = glyphWidth + spacing;
        }

        // NOTE: When wordWrap is ON we first measure how much of the text we can draw before going outside of the rec container
        // We store this info in startLine and endLine, then we change states, draw the text between those two variables
        // and change states again and again recursively until the end of the text (or until we get outside of the container).
        // When wordWrap is OFF we don't need the measure state so we go to the drawing state immediately
        // and begin drawing on the next line before we can get outside the container.
        if (state == MEASURE_STATE)
        {
            if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n'))
                endLine = i;

            if ((textOffsetX + glyphWidth) > rec.width)
            {
                endLine = (endLine < 1) ? i : endLine;
                if (i == endLine)
                    endLine -= codepointByteCount;
                if ((startLine + codepointByteCount) == endLine)
                    endLine = (i - codepointByteCount);

                state = !state;
            }
            else if ((i + 1) == length)
            {
                endLine = i;
                state = !state;
            }
            else if (codepoint == '\n')
                state = !state;

            if (state == DRAW_STATE)
            {
                textOffsetX = 0;
                i = startLine;
                glyphWidth = 0;

                // Save character position when we switch states
                i16 tmp = lastk;
                lastk = k - 1;
                k = tmp;
            }
        }
        else
        {
            if (codepoint == '\n')
            {
                if (!wordWrap)
                {
                    textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                    textOffsetX = 0;
                }
            }
            else
            {
                if (!wordWrap && ((textOffsetX + glyphWidth) > rec.width))
                {
                    textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                    textOffsetX = 0;
                }

                // When text overflows rectangle height limit, just stop drawing
                if ((textOffsetY + font.baseSize * scaleFactor) > rec.height)
                    break;

                // Draw selection background
                bool isGlyphSelected = false;
                if ((selectStart >= 0) && (k >= selectStart) && (k < (selectStart + selectLength)))
                {
                    DrawRectangleRec((Rectangle){rec.x + textOffsetX - 1, rec.y + textOffsetY, glyphWidth, (float)font.baseSize * scaleFactor}, selectBackTint);
                    isGlyphSelected = true;
                }

                if ((codepoint != ' ') && (codepoint != '\t'))
                {
                    DrawTextCodepoint(font, codepoint, (Vector2){rec.x + textOffsetX, rec.y + textOffsetY}, fontSize, isGlyphSelected ? selectTint : tint);
                }
            }

            if (wordWrap && (i == endLine))
            {
                textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                textOffsetX = 0;
                startLine = endLine;
                endLine = -1;
                glyphWidth = 0;
                selectStart += lastk - k;
                k = lastk;

                state = !state;
            }
        }

        if ((textOffsetX != 0) || (codepoint != ' '))
            textOffsetX += glyphWidth;
    }
}

void town_view_render()
{
    switch (town_sub_states)
    {
    case ts_main:
    {
        static rect_t town_text_box_background = {20, 20, GAME_WIDTH - 40, 70};
        static rect_t town_text_box = {30, 30, GAME_WIDTH - 40, 50};
        static rect_t town_choice_box_background = {20, 200, GAME_WIDTH / 2 - 40, 150};
        static rect_t town_choice_box = {30, 210, GAME_WIDTH - 40, 50};

        DrawRectangleRec(town_text_box_background, (Color){50, 50, 50, 150});
        text_boxed_draw("You have arrived at the town of TOWN_NAME_PLACEHOLDER!", town_text_box, 20, 10, true, WHITE);

        DrawRectangleRec(town_choice_box_background, (Color){50, 50, 50, 150});
        text_boxed_draw("Do you: ", town_choice_box, 20, 10, true, WHITE);

        static i32 menu_offset_x = 30;
        static i32 menu_offset_y = 250;
        static i32 menu_font_size = 21;
        static i32 menu_spacing = 32;

        for (u32 i = 0; i < NUM_TOWN_SUB_STATES; i++)
        {
            DrawText(town_selection_strings[i],
                     menu_offset_x, menu_spacing * i + menu_offset_y, menu_font_size,
                     i == town_selection_state.selection ? YELLOW : GRAY);
        }
    }
    break;
    case ts_tavern:
    {
        tavern_render();
    }
    break;
    case ts_shipyard:
    {
        shipyard_render();
    }
    break;
    case ts_merchant:
    {
        merchant_render();
    }
    break;
    }
}