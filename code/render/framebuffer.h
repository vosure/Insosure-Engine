#pragma once

#include <stdio.h>

// NOTE(insolence): We should call glDeleteFramebuffers(1, &ID); but where? Check for memory leaks
struct framebuffer
{
    unsigned int ID;
    unsigned int TextureAttachment[2];
    unsigned int RBO;
};

global_variable framebuffer PostprocessingFB;
global_variable framebuffer HdrFB;
global_variable framebuffer PingpongFB[2]; // NOTE(insolence): Framebuffers for Gaussian blur

void
AttachTexture(framebuffer *Framebuffer, int ScreenWidth, int ScreenHeight, int InternalFormat, int Index)
{
    glGenTextures(1, &Framebuffer->TextureAttachment[Index]);
    glBindTexture(GL_TEXTURE_2D, Framebuffer->TextureAttachment[Index]);

    glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); // NOTE(insolence): For HDR we use GL_RGBA16F Internal format

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer->ID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + Index, GL_TEXTURE_2D, Framebuffer->TextureAttachment[Index], 0);
}

#define MAKE_RENDERBUFFER true
#define RENDERBUFFER_NEEDLESS true

void
AttachRBO(framebuffer *Framebuffer, int ScreenWidth, int ScreenHeight)
{
    glGenRenderbuffers(1, &Framebuffer->RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, Framebuffer->RBO);

    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer->ID);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, ScreenWidth, ScreenHeight);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Framebuffer->RBO);
}

framebuffer
CreateFramebuffer(int ScreenWidth, int ScreenHeight, int InternalFormat, int TextureAttachmentsNum, bool AttachRB)
{
    framebuffer Framebuffer = {};

    glGenFramebuffers(1, &Framebuffer.ID);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer.ID);

    for (int i = 0; i < TextureAttachmentsNum; i++)
    {
        AttachTexture(&Framebuffer, ScreenWidth, ScreenHeight, InternalFormat, i);
    }
    if (TextureAttachmentsNum == 2)
    {
        unsigned int Attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, Attachments);
    }

    if (AttachRB)
        AttachRBO(&Framebuffer, ScreenWidth, ScreenHeight);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer creation failed!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return Framebuffer;
}

internal void 
MakeFramebuffers(int Width, int Height)
{
    if (PostprocessingFB.ID)
    {
        glDeleteFramebuffers(1, &PostprocessingFB.ID);
        glDeleteFramebuffers(1, &HdrFB.ID);
        glDeleteFramebuffers(1, &PingpongFB[0].ID);
        glDeleteFramebuffers(1, &PingpongFB[1].ID);
    }

    PostprocessingFB = CreateFramebuffer(Width, Height, GL_RGBA, 1, MAKE_RENDERBUFFER);
    HdrFB = CreateFramebuffer(Width, Height, GL_RGB16F, 2, MAKE_RENDERBUFFER);
    for (int i = 0; i < 2; i++)
    {
        PingpongFB[i] = CreateFramebuffer(Width, Height, GL_RGB16F, 1, RENDERBUFFER_NEEDLESS);
    }
}

