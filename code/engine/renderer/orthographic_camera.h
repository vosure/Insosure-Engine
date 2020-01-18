#pragma once

// TODO(insolence): Implement rotations of camera
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
    Camera->ViewMatrix = Translate(Camera->Position); // * Rotate(Camera->Rotation, {0.f, 0.f, 1.f});

    Camera->ViewProjection = Camera->ProjectionMatrix * Camera->ViewMatrix;
}

void
SetViewProjection(orthographic_camera *Camera, float Left, float Right, float Bot, float Top)
{
    Camera->ProjectionMatrix = Ortho(Bot, Top, Left, Right, -1.f, 1.f);
    RecalculateViewMatrix(Camera);
}
