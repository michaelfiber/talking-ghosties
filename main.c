/*******************************************************************************************
 *
 *   Talking Ghosties - two ghosts are drawn with mouths that open and close according to the
 *   left and right channels of whatever mp3 they are set to play.
 *
 *   Based on example: https://github.com/raysan5/raylib/blob/master/examples/audio/audio_mixed_processor.c
 *
 *   Example originally created with raylib 4.2, last time updated with raylib 4.2
 *
 *   Example contributed by hkc (@hatkidchan) and reviewed by Ramon Santamaria (@raysan5)
 *
 *   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
 *   BSD-like license that allows static linking with closed source software
 *
 *   Copyright (c) 2023 michaelfiber (@michaelfiber)
 *
 ********************************************************************************************/

#include "raylib.h"
#include <math.h>

static float exponent = 1.0f; // Audio exponentiation value

static float l_vol = 0.0f;
static float r_vol = 0.0f;
Color faded_black = (Color){
    0,
    0,
    0,
    30};

void process_audio(void *buffer, unsigned int frames)
{
    float *samples = (float *)buffer; // Samples internally stored as <float>s

    l_vol = 0.0f;
    r_vol = 0.0f;

    for (unsigned int frame = 0; frame < frames; frame++)
    {
        float *left = &samples[frame * 2 + 0], *right = &samples[frame * 2 + 1];

        *left = powf(fabsf(*left), exponent) * ((*left < 0.0f) ? -1.0f : 1.0f);
        *right = powf(fabsf(*right), exponent) * ((*right < 0.0f) ? -1.0f : 1.0f);

        l_vol += fabsf(*left) / frames; // accumulating average volume
        r_vol += fabsf(*right) / frames;
    }
}

void draw_ghost(int x, int y, float mouth_size)
{
    DrawCircle(x, y - 50, 100, WHITE);
    DrawRectangle(x - 100, y - 50, 200, 200, WHITE);

    // left eye
    DrawCircle(x - 50, y - 75, 20, BLACK);
    DrawCircle(x - 45, y - 70, 15, WHITE);

    // right eye
    DrawCircle(x + 50, y - 75, 20, BLACK);
    DrawCircle(x + 55, y - 70, 15, WHITE);

    // mouth
    float height = 10 + mouth_size * 800;
    if (height > 100)
        height = 100;
    DrawRectangle(x - 50, y, 100, height, BLACK);
}

int main(void)
{
    float offset = 0.0f;
    const int screen_width = 800;
    const int screen_height = 450;

    InitWindow(screen_width, screen_height, "talking ghosties");

    InitAudioDevice();

    AttachAudioMixedProcessor(process_audio);

    Music music = LoadMusicStream("resources/soundtrack.mp3");

    PlayMusicStream(music);

    SetTargetFPS(60);

    // do 1 initial full clear and then every subsequent clear will use partially translucent
    // black for ghosty fade effect.
    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        offset += GetFrameTime();

        BeginDrawing();
        {
            DrawRectangle(0, 0, screen_width, screen_height, faded_black);
            draw_ghost(screen_width / 3 + cosf(offset) * 10, screen_height / 2 + sinf(offset) * 50, l_vol);
            draw_ghost(screen_width / 3 * 2 + cosf(offset + 1) * 20, screen_height / 2 + sinf(offset + 1) * 50, r_vol);
        }
        EndDrawing();
    }

    UnloadMusicStream(music);
    DetachAudioMixedProcessor(process_audio);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
