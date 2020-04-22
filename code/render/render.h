#pragma once

#include "math/linear_math.h"
#include "orthographic_camera.h"
#include "texture.h"
#include "shader.h"

void DrawRectangle(float FromX, vec2 From, vec2 To, color Color);
void DrawRectangleTextured(orthographic_camera *Camera, mat4 Transform, uint Texture, uint NormalMap,
                           std::vector<dir_light> DirLights, std::vector<point_light> PointLights, std::vector<spotlight_light> SpotLights, color Color = vec3{0, 0, 0});
void RenderScreenTexture();
void PostprocessScreenTexture(int FBTexture, postprocessing_effects Effects);
void ApplyHDR(int ScreenTexture, int BloomTexture, float Exposure);

void InstancedDrawRectangleTextured(orthographic_camera *Camera, vec2 *Offsets, int Amount, uint Texture, color Color);
