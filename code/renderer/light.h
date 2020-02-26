#pragma once

struct dir_light
{
    vec3 Direction;

    color Ambient;
    color Diffuse;
    color Specular;
};

struct point_light
{
    vec3 Position;

    color Ambient;
    color Diffuse;
    color Specular;

    vec3 CLQ; // NOTE(vosure): Constant, Linear, Quadratic coefs.
};

struct spotlight_light
{
    vec3 Position;
    vec3 Direction;

    float CutOff;
    float OuterCutOff;

    color Ambient;
    color Diffuse;
    color Specular;

    vec3 CLQ;
};

dir_light
DirLight(vec3 Direction, color Ambient, color Diffuse, color Specular)
{
    dir_light Light;

    Light.Direction = Direction;

    Light.Ambient = Ambient;
    Light.Diffuse = Diffuse;
    Light.Specular = Specular;

    return Light;
}

point_light
PointLight(vec3 Position, color Ambient, color Diffuse , color Specular, vec3 CLQ)
{
    point_light Light;

    Light.Position = Position;

    Light.Ambient = Ambient;
    Light.Diffuse = Diffuse;
    Light.Specular = Specular;

    Light.CLQ = CLQ;

    return Light;
}

spotlight_light
SpotLight(vec3 Position, vec3 Direction, float CutOff, float OuterCutOff, color Ambient, color Diffuse, color Specular, vec3 CLQ)
{
    spotlight_light Light;

    Light.Position = Position;
    Light.Direction = Direction;

    Light.CutOff = CutOff;
    Light.OuterCutOff = OuterCutOff;

    Light.Ambient = Ambient;
    Light.Diffuse = Diffuse;
    Light.Specular = Specular;

    Light.CLQ = CLQ;

    return Light;
}
