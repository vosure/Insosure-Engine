#pragma once

// TODO(insolence): Probably implement 3D Perlin noise?

#include <math.h>

// A lattice grid of size 256
global_variable const unsigned char HASH[256] = {
    208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
    185,248,251,245,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
    9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
    70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
    203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
    164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
    228,108,245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
    232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
    193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
    101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
    135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
    114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219
};

internal int
Noise2(int X, int Y)
{
    const int SEED = 1985;

    int YIndex = (Y + SEED) % 256; // NOTE(insolence): 256 is a period of a func
    if (YIndex < 0)
        YIndex += 256;
    int XIndex = (HASH[YIndex] + X) % 256;
    if (XIndex < 0)
        XIndex += 256;
    int Result = HASH[XIndex];
    
    return Result;
}

internal double 
LinInter(double X, double Y, double T)
{
    return (X + T*(Y-X));
}

// NOTE(insolence): A smoothstep remapping function for T
internal double
SmoothInter(double X, double Y, double T)
{
    float TRemapSmoothstep = T*T * (3 - 2*T); // NOTE(insolence): T * T * T * (T * (T * 6 - 15) + 10); Fade func as defined by Ken Perlin
    return LinInter(X, Y, TRemapSmoothstep);
}

double
Noise2D(double X, double Y)
{
    int Xint = floor(X);
    int Yint = floor(Y);
    double XFrac = X - Xint;
    double YFrac = Y - Yint;
    int S = Noise2(Xint, Yint);
    int T = Noise2(Xint + 1, Yint);
    int U = Noise2(Xint, Yint + 1);
    int V = Noise2(Xint + 1, Yint + 1);
    double Low = SmoothInter(S, T, XFrac);
    double High = SmoothInter(U, V, XFrac);
    double Result = SmoothInter(Low, High, YFrac);

    return Result;
}

// NOTE(insolence): Frequency is the period of the function
double
PerlinGet2D(double X, double Y, double Freq, int Depth)
{
    double Xa = X * Freq;
    double Ya = Y * Freq;
    double Amp = 1.0;
    double Fin = 0.0;
    double Div = 0.0;
    for (int i = 0; i < Depth; i++)
    {
        Div += 256 * Amp;
        Fin += Noise2D(Xa, Ya) * Amp;
        Amp /= 2;
        Xa *= 2;
        Ya *= 2;
    }

    return Fin/Div;
}


////////////////////////////////////////////
///// 1D Value (Not Perlin!!!) Noise ///////
////////////////////////////////////////////

global_variable double SlopeAt[256];

double
ValueNoise1D(double X)
{
    int MaxVertices = 256;
    int MaxVerticesMask = 255;

    for (int i = 0; i < MaxVertices; i++)
    {
        SlopeAt[i] = ((double)GetRandomInt(0, MaxVertices)) / 256.0;
    }

    int XInt = (int)X - (X < 0 && X != (int)X);
    float T = X - XInt;

    int XMin = XInt & MaxVerticesMask;
    int XMax = (XMin + 1) & MaxVerticesMask;

    return LinInter(SlopeAt[XMin], SlopeAt[XMax], T);
}