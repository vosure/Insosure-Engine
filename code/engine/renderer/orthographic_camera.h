#pragma once

// TODO(insolence): Probably implement Zooming in and out
struct orthographic_camera
{
    mat4 ProjectionMatrix;
    mat4 ViewMatrix;
    mat4 ViewProjection;

    vec3 Position = {};
    float Rotation = 0.f;
};
typedef orthographic_camera camera_2D;

void
RecalculateViewMatrix(orthographic_camera *Camera)
{
    Camera->ViewMatrix = Rotate(Camera->Rotation, {0.f, 0.f, 1.f}) * Translate(Camera->Position);

    Camera->ViewProjection = Camera->ViewMatrix * Camera->ProjectionMatrix;
}

void
SetViewProjection(orthographic_camera *Camera, float Left, float Right, float Bot, float Top)
{
    Camera->ProjectionMatrix = Ortho(Bot, Top, Left, Right, -1.f, 1.f);
    RecalculateViewMatrix(Camera);
}
