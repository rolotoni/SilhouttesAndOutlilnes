///////////////////////////////////////////////////////////////////////
// Provides the creation of a Frame Buffer Object
//
// Gary Herron
//
// Copyright © 2011 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////
#ifndef  __FBO__
#define  __FBO__

class FBO {
public:
    int width, height;
    unsigned int fbo;
    unsigned int textures[4];
    int nbuffers;
    unsigned int buffers[4];

    void CreateFBO(const int w, const int h, const int NumBuffers);
    void Bind();
    void Unbind();
};

#endif
