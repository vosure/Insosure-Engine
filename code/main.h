#pragma once

#include <iostream>
#include <vector>
#include <stdio.h>

#include "math/linear_math.h"
#include "renderer/orthographic_camera.h"

#define internal static
#define local_persist static
#define global_variable static
#define bool32 int
#define uint unsigned int

#if 0
    #define real32 float
    #define real64 double
#endif

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

// NOTE(insolence): 1440 x 900 (8 x 5 proportion) for NV.
#if INSOLENCE
    global_variable int WINDOW_WIDTH = 1920;
    global_variable int WINDOW_HEIGHT = 1080;
#elif VOSURE
    global_variable int WINDOW_WIDTH = 1280;
    global_variable int WINDOW_HEIGHT = 720;
#endif

global_variable const float RESOLUTION = 16.f/9.f;

global_variable int SCREEN_WIDTH;
global_variable int SCREEN_HEIGHT;

global_variable int CurrentWidth;
global_variable int CurrentHeight;

global_variable bool IsFullscreen = false;

global_variable const int WORLD_WIDTH = 300;
global_variable const int WORLD_HEIGHT = 300;

struct unit
{
    vec2 Pos;
    vec2 OldPos;
    int Power;

    // uint Texture;
};

struct player
{
    const static int UnitsNum = 2;
    unit Units[UnitsNum];
    int UnitChosen; // NOTE(insolence): Which unit we currently control, if -1 then no unit chosen
};
#define NO_UNIT -1

struct tile
{
    int Value;
    bool Occupied;
};
#define EMPTY_TILE 0
#define GHOST 2
#define MONSTER 3
#define TREASURE 4
#define OBSTACLE 5

// TODO(insolence): Later make these little mini-maps of their own
struct battlefield
{
    orthographic_camera BattleCamera;
    vec2 BattleLocation; // NOTE(insolence): Position of a battlefield on a global map

    tile Field[10][10];
    vec2 PlayerPos;
    vec2 OldPlayerPos;
};

enum game_mode
{
    MENU = 0,
    GLOBAL = 1,
    BATTLE = 2,
};

// NOTE(insolence): Game state
struct game_world
{
    game_mode Mode;
    tile Tiles[WORLD_WIDTH][WORLD_HEIGHT];
    player Player;

    std::vector<battlefield> ActiveBattlefields;
    int ActiveBattleNum; // NOTE(insolence): -1 if no battle
};
#define NO_BATTLE -1