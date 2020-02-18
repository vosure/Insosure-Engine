#pragma once

struct point_light
{
    vec3 Position;
    color Color;
    float Radius; // NOTE(insolence): Normally Radius and Intensity are within 0-2 range
    float Intensity;
};

point_light
PointLight(vec3 Position, color Color, float Radius, float Intensity)
{
    point_light Light;
    Light.Position = Position;
    Light.Color = Color;
    Light.Radius = Radius;
    Light.Intensity = Intensity;

    return Light;
}
