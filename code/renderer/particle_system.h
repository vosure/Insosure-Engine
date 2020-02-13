#pragma once

struct particle
{
    vec2 Position;
    vec2 Velocity;
    float Size;
    float Lifetime;
    float InitialLifetime;
};


particle
SpawnParticle(vec2 Pos, float RandomOffset, vec2 Velocity, float InitialLifetime, float Size)
{
    particle Particle;

    vec2 Random = vec2{GetRandomFloat(-RandomOffset, RandomOffset), GetRandomFloat(-RandomOffset, RandomOffset)};
    Particle.Position = Pos + Random;
    Particle.Velocity = Velocity;
    Particle.Size = Size;
    Particle.InitialLifetime = InitialLifetime;
    Particle.Lifetime = InitialLifetime;

    return Particle;
}

// NOTE(insolence): Updating lifetime and position of a particle
void
UpdateParticles(std::vector<particle> &Particles, float DeltaTime)
{
    for (int i = 0; i < Particles.size(); i++)
    {
        Particles[i].Position += Particles[i].Velocity * DeltaTime;
        Particles[i].Lifetime -= DeltaTime;
        if (Particles[i].Lifetime <= 0.f)
            Particles.erase(Particles.begin() + i);
    }
}