///////////////////////////////////////////////////////////////////////
// Provides for the creation and use of shader programs.
//
// Gary Herron
//
// Copyright © 2011 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

class ShaderProgram
{
public:
    int program;
    
    void CreateProgram();
    void CreateShader(const char* fileName, const int type);
    void LinkProgram();
    void Use();
    void Unuse();
    void ActivateTexture(const int unit, const char* name, const int texture);
};
