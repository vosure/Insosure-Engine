#pragma once

#include <stdio.h>

// NOTE(insolence): We should call glDeleteFramebuffers(1, &ID); but where? Check for memory leaks
struct framebuffer
{
    unsigned int ID;
    unsigned int TextureAttachment;
    unsigned int RBO;
};

void 
AttachTexture(framebuffer *Framebuffer, int ScreenWidth, int ScreenHeight)
{
    glGenTextures(1, &Framebuffer->TextureAttachment);
    glBindTexture(GL_TEXTURE_2D, Framebuffer->TextureAttachment);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  

    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer->ID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Framebuffer->TextureAttachment, 0);
}

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
CreateFramebuffer(int ScreenWidth, int ScreenHeight)
{
    framebuffer Framebuffer;
    Framebuffer.TextureAttachment = 0;
    Framebuffer.RBO = 0;

    glGenFramebuffers(1, &Framebuffer.ID);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer.ID);

    AttachTexture(&Framebuffer, ScreenWidth, ScreenHeight);
    AttachRBO(&Framebuffer, ScreenWidth, ScreenHeight);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer creation failed!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return Framebuffer;
}

