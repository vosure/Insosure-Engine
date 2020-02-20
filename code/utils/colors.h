#pragma once

typedef vec3 color;
typedef vec4 color4;

// NOTE(insolence): We can exceed the 1.f limit cause of HDR, so we can just multiply a color by some amount
const color RED          = {1, 0, 0};
const color BLUE         = {0, 0, 1};
const color GREEN        = {0, 0.5f, 0}; // NOTE(insolence): LIME is 1, GREEN is 0.5
const color LIME         = {0, 1, 0};
const color MAGENTA      = {1, 0, 1};
const color YELLOW       = {1, 1, 0};
const color WHITE        = {1, 1, 1};
const color BLACK        = {0, 0, 0};
const color GREY         = {0.5f, 0.5f, 0.5f};
const color CYAN         = {0, 1, 1};
const color AQUA         = {0, 1, 1};
const color SILVER       = {0.75f, 0.75f, 0.75f};
const color MAROON       = {0.5f, 0, 0};
const color OLIVE        = {0.5f, 0.5f, 0};
const color PURPLE       = {0.5f, 0, 0.5f};
const color TEAL         = {0, 0.5f, 0.5f};
const color NAVY         = {0, 0, 0.5f};
const color CORAL        = {1, 0.5f, 0.35f};
const color GOLD         = {1, 0.83f, 0};
const color CRIMSON      = {0.8f, 0.08f, 0.24f};
const color BROWN        = {0.65f, 0.15f, 0.15f};
const color ORANGE       = {1, 0.65f, 0};
const color KHAKI        = {0.9f, 0.85f, 0.55f};
const color DARK_GREEN   = {0, 0.35f, 0};
const color SPRING_GREEN = {0, 1, 0.5f};
const color DARK_BLUE    = {0, 0, 0.55f};
const color INDIGO       = {0.33f, 0, 0.5f};
const color VIOLET       = {0.95f, 0.47f, 0.95f};
const color DEEP_PINK    = {1.f, 0.08f, 0.54f};
const color PINK         = {1.f, 0.75f, 0.8f};
const color CHOCOLATE    = {0.8f, 0.4f, 0.1f};
const color DIM_GRAY     = {0.4f, 0.4f, 0.4f};
const color LIGHT_GRAY   = {0.8f, 0.8f, 0.8f};
const color WHITE_SMOKE  = {0.94f, 0.94f, 0.94f};
const color LAWN_GREEN   = {0.5f, 0.99f, 0};
const color YELLOW_GREEN = {0.6f, 0.8f, 0.18f};


const color4 TRANSPARENT4 = {1, 1, 1, 0};

void
ColorToString(color Color)
{
    printf("R: %.2f, G: %.2f, B: %2f \n", Color.R, Color.G, Color.B);
}