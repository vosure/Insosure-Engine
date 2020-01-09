#pragma once

#include "../math/linear_math.h"

struct orthographic_camera
{
    mat4 ProjectionMatrix;
    mat4 ViewMatrix;
    mat4 ViewProjection;

    vec3 Position = {}; 
    float Rotation = 0.f; // TODO(insolence): Maybe implement later, unnecessary right now 
};
typedef orthographic_camera camera_2D;

void
RecalculateViewMatrix(orthographic_camera *Camera)
{
    Camera->ViewMatrix = Identity();
    mat4 Transform = Translate(Camera->Position); // * Rotate();
    Camera->ViewMatrix = Transform;
    Camera->ViewProjection = Camera->ProjectionMatrix * Camera->ViewMatrix;
}

void
SetProjection(orthographic_camera *Camera, float Left, float Right, float Bot, float Top)
{
    Camera->ProjectionMatrix = Ortho(Bot, Top, Left, Right, -1.f, 1.f);

    RecalculateViewMatrix(Camera);
    Camera->ViewProjection = Camera->ProjectionMatrix * Camera->ViewMatrix;
}
