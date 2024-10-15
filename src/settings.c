#include "include/common.h"
#include "include/settings.h"
#include "include/custom_string.h"

char *settings_file_path = "./settings.ini";

// Creates the default settings.ini file
void create_settings_file(settings_s settings)
{
    printf("Creating default settings.ini file!\n");
    FILE *fp = fopen(settings_file_path, "w");

    // For creating the base settings.ini file
    fprintf(fp, "; comments with \";\"\n");
    fprintf(fp, "\n");
    fprintf(fp, "; resolution settings\n");
    fprintf(fp, "WINDOW_WIDTH=1920\n");
    fprintf(fp, "WINDOW_HEIGHT=1080\n");
    fprintf(fp, "FULLSCREEN=FALSE\n");
    fprintf(fp, "\n");
    fprintf(fp, "; volume settings\n");
    fprintf(fp, "VOLUME_ALL=100\n");
    fprintf(fp, "VOLUME_WAVES=100\n");
    fprintf(fp, "VOLUME_EFFECTS=100\n");
    fprintf(fp, "VOLUME_INVENTORY=100\n");

    fclose(fp);
}

// Reads the settings from the settings.ini file
settings_s read_settings()
{
    // Default settings
    settings_s settings_default = {
        .window_width = 960,
        .window_height = 540,
        .fullscreen = false,
        .volume_all = 100,
        .volume_effects = 100,
        .volume_inventory = 100,
        .volume_waves = 100,
    };

    // Open settings file
    FILE *fp = fopen(settings_file_path, "r");

    // If we fail to open, create the file with standard values and use them
    if (fp == NULL)
    {
        create_settings_file(settings_default);

        return settings_default;
    }

    settings_s red_settings = {0};
    char buffer[100];
    char *success = fgets(buffer, 100, fp);
    while (success != NULL)
    {
        if (59 == success[0]) // 59 == ;
        {
            success = fgets(buffer, 100, fp);
            continue;
        }
        else
        {
            int length = string_length(success);

            for (int i = 0; i < length; i++)
            {
                if (61 == success[i]) // 61 == =
                {
                    int substr_length = string_length(success + i) - 2; // -3 takes away: '=' and '\n'
                    char *key = string_get_substring(success, 0, i + 1);
                    char *value = string_get_substring(success, i + 1, substr_length);

                    // Add settings under here!
                    if (string_compare(key, "WINDOW_WIDTH"))
                        red_settings.window_width = atoi(value);
                    if (string_compare(key, "WINDOW_HEIGHT"))
                        red_settings.window_height = atoi(value);
                    if (string_compare(key, "VOLUME_ALL"))
                        red_settings.volume_all = atof(value);
                    if (string_compare(key, "VOLUME_WAVES"))
                        red_settings.volume_waves = atof(value);
                    if (string_compare(key, "VOLUME_EFFECTS"))
                        red_settings.volume_effects = atof(value);
                    if (string_compare(key, "VOLUME_INVENTORY"))
                        red_settings.volume_inventory = atof(value);
                    if (string_compare(key, "FULLSCREEN"))
                        red_settings.fullscreen = string_compare(value, "TRUE") ? true : false;
                }
            }
            success = fgets(buffer, 100, fp);
            continue;
        }
    }

    // Display the settings (mostly for knowing if they parsed good)
    printf("Settings:\n");
    printf("ww: %i\n", red_settings.window_width);
    printf("wh: %i\n", red_settings.window_height);
    printf("fs: %s\n", red_settings.fullscreen == true ? "true" : "false");
    printf("va: %f\n", red_settings.volume_all);
    printf("ve: %f\n", red_settings.volume_effects);
    printf("vi: %f\n", red_settings.volume_inventory);
    printf("vw: %f\n", red_settings.volume_waves);

    return red_settings;
}

const u32 settings_amount = 7;
settings_rec_t settings_buttons[settings_amount] = {
    // Volume : All
    {
        .resolution = 20,
        .type = settings_type_volume,
        .hover_text = "Change the volume for all output",
        .text = "Volume",
        .max_val = 100.0f,
        .min_val = 0.0f,
    },
    // Volume : Waves
    {
        .resolution = 20,
        .type = settings_type_volume,
        .hover_text = "Change the wave volume",
        .text = "Waves",
        .max_val = 100.0f,
        .min_val = 0.0f,
    },
    // Volume : Effects
    {
        .resolution = 20,
        .type = settings_type_volume,
        .hover_text = "Change the effects volume",
        .text = "Effects",
        .max_val = 100.0f,
        .min_val = 0.0f,
    },
    // Volume : Inventory
    {
        .resolution = 20,
        .type = settings_type_volume,
        .hover_text = "Change the inventory volume",
        .text = "Inventory",
        .max_val = 100.0f,
        .min_val = 0.0f,
    },
    // Window : Fullscreen
    {
        .type = settings_type_fullscreen,
        .hover_text = "Toggle fullscreen",
        .text = "Fullscreen",
    },
    // Window : Window width
    {
        .resolution = 16,
        .hover_text = "Window resolution",
        .text = "Resolution",
        .type = settings_type_resolution,
    },
};

void settings_page_render()
{
    vec2_t screen_middle = {GAME_WIDTH / 2, GAME_HEIGHT / 2};
    vec2_t button_size = {150, 50};

    int i = 0;
    int rows = 4;
    int cols = ceilf(settings_amount / 3);

    int x_i = 0;
    int start_x = screen_middle.x - (cols * 160) - 75;
    int x = start_x;
    int y = 100;

    vec2_t mouse_pos = get_virtual_mouse();

    DrawText("TO BE IMPLEMENTED", 400, 50, 24, WHITE);
    while (i < settings_amount)
    {
        DrawRectangle(x, y, button_size.x, button_size.y, GRAY);

        if (CheckCollisionPointRec(mouse_pos, (rect_t){x, y, button_size.x, button_size.y}))
        {
            DrawRectangle(x, y, button_size.x, button_size.y, DARKGRAY);
        }

        DrawText(settings_buttons[i].text, x, y, 16, WHITE);

        i++;
        x_i++;

        if (x_i > cols)
        {
            x = start_x;
            x_i = 0;
            y += 75;
        }
        else
        {
            x += button_size.x + 150;
        }
    }
}