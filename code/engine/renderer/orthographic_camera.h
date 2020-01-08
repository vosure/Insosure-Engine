#pragma once

// NOTE(insolence): Temp structs until the math materializes
struct vec3
{
    float X, Y, Z;
};
struct mat4
{
    float values[4][4];
};


struct orthographic_camera
{
    mat4 ProjectionMatrix;
    mat4 ViewMatrix;
    mat4 ViewProjection;

    vec3 Position; 
    float Rotation = 0.f; // TODO(insolence): Maybe implement later, unnecessary right now 
};
typedef orthographic_camera camera_2D;

void
SetProjection(orthographic_camera *Camera, float Left, float Right, float Bottom, float Top)
{
    // mat4 ProjectionMatrix = ;
    // Camera->ProjectionMatrix = ProjectionMatrix;
    // Camera->ViewProjection = Camera->ProjectionMatrix * Camera->ViewMatrix;
}

void
RecalculateViewMatrix(orthographic_camera *Camera)
{
    // ViewMatrix = ;
    // Camera->ViewProjection = Camera->ProjectionMatrix * ViewMatrix;
}