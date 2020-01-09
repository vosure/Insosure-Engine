#pragma once

#include "math/linear_math.h"
#include "orthographic_camera.h"
#include "texture.h"
#include "shader.h"

void DrawRectangle(float FromX, vec2 From, vec2 To, color Color);
void DrawRectangleTextured(orthographic_camera *Camera, vec2 From, vec2 To, texture Texture, color Color);
