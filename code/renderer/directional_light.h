#pragma once

struct directional_light
{
    vec3 Position;
    color Color;
    float Intensity;
};

directional_light
DirLight(vec3 Position, color Color) //, float Intensity)
{
    directional_light Light;
    Light.Position = Position;
    Light.Color = Color;
    //Light.Intensity = Intensity;

    return Light;
}

directional_light
NullLight() //, float Intensity)
{
    return DirLight({0, 0, 0}, {0, 0, 0});
    //Light.Intensity = Intensity;
}