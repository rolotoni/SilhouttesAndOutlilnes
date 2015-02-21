///////////////////////////////////////////////////////////////////////
// Provides the creation of a Frame Buffer Object
//
// Gary Herron
//
// Copyright © 2011 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////


#include <fstream>

#include "fbo.h"
#include "scene.h"
#include "GLee.h"

// This class is a small wrapper around the creation and use of an
// OpenGL Frame Buffer Object (FBO).  It will create an FBO with
// texture of the specified size.  When bound, the graphics pipeline
// will use the texture as the render target.  When not bound the
// texture is available to be used as any normal texture.

// To create MULTIPLE render targets, make the changes indicated with
// a @@ symbol.


void FBO::CreateFBO(const int w, const int h, const int NumBuffers)
{
    width = w;
    height = h;
    nbuffers = NumBuffers;               // @@: Number of render targets

    glGenFramebuffersEXT(1, &fbo);  
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
        
    // Create a render buffer, and attach it to FBO's depth attachment
    unsigned int depthBuffer;
    glGenRenderbuffersEXT(1, &depthBuffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,
                             width, height);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, depthBuffer);

    // @@: Create texture ID for each render target.
    glGenTextures(nbuffers, textures);

    // Repeat this block for each render target.
	for(unsigned i=0; i < nbuffers; ++i)
	{
      // Create texture and attach to the FBO's color 0 attachment
      glBindTexture(GL_TEXTURE_2D, textures[i]);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height,
                   0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	  
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);        
	  
      // Attach the ith render target to GL_COLOR_ATTACHMENT0_EXT+i
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                GL_COLOR_ATTACHMENT0_EXT+i,
                                GL_TEXTURE_2D, textures[i], 0);
	  
      buffers[i] = GL_COLOR_ATTACHMENT0_EXT+i;
	}
    // End of repeatable block

    // Check for completeness/correctness
    int status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
        printf("FBO Error: %d\n", status);

    // Unbind the fbo.  
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}


void FBO::Bind() 
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo); 
    glDrawBuffers(nbuffers, buffers);
}

void FBO::Unbind()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
