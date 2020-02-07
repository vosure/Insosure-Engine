#pragma once

struct particle
{
    vec2 Position;
    vec2 Velocity;
    float Size;
    float Lifetime;
};

particle
SpawnParticle(vec2 ObjectPos, vec2 Offset)
{
    //vec2 Random = { LehmerFloat() * 3.f, LehmerFloat() * 2.f };
    vec2 Random = {((rand() % 100) - 50) / 100.0f, ((rand() % 100) - 50) / 100.0f};

    particle Particle;
    Particle.Position = ObjectPos + Random + Offset;
    Particle.Lifetime = 6.0f;
    // Particle.Velocity =LehmerFloat() * 1.5fObject.Velocity * 0.1f;
    Particle.Size = LehmerFloat() * 2.f;

    return Particle;
}

void
UpdateParticleLifetime(particle *Particles, int Length, float DeltaTime)
{
    for (int i = 0; i < Length; i++)
    {
        Particles[i].Lifetime -= DeltaTime;
        if (Particles[i].Lifetime <= 0.f)
            Particles[i].Lifetime = 0.f;
    }
}