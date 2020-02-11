#pragma once
#include <iostream>

#include <stdio.h>
#include "math/linear_math.h"

#define internal static
#define local_persist static
#define global_variable static
#define bool32 int
#define uint unsigned int

// NOTE(insolence): Useful Macros
#define Assert(Expression) if (!(Expression)) { *(int*)0 = 0; }
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
#define Swap(x, y, T) do { T tmp = (x); (x) = (y); (y) = tmp; } while(0)

#define NumToStr(x) str(x) // printf(NumTostr(20));
#define str(x) #x

// assert(IMPLIES(n > 0, array != NULL));
#define IMPLIES(x, y) (!(x) || (y))

// TODO: Check how this works
#define foreach(item, array) \
    for(int keep=1, \
            count=0,\
            size=sizeof (array)/sizeof *(array); \
        keep && count != size; \
        keep = !keep, count++) \
      for(item = (array)+count; keep; keep = !keep)


struct postprocessing_effects
{
    bool Inversion;
    bool Grayscale;
    bool Blur;
};

global_variable const int WINDOW_WIDTH = 1920; // NOTE(insolence): 1920 x 1080 is normal, 1440 x 900 (8 x 5 proportion) for NV
global_variable const int WINDOW_HEIGHT = 1080;
global_variable int SCREEN_WIDTH;
global_variable int SCREEN_HEIGHT;

global_variable bool IsFullscreen = false;

struct player
{
    vec2 Pos;
    vec2 OldPos;
    int Power;
};

struct tile
{
    int Value;
    bool Visible; // NOTE(insolence): Is it covered by the fog of war?
};

// NOTE(insolence): Game state
struct game_world
{
    tile Tiles[300][300];
    player Player;
};