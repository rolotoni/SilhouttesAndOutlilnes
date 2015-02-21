///////////////////////////////////////////////////////////////////////
// Provides for the creation and use of shader programs.
//
// Gary Herron
//
// Copyright Å© 2011 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

#include "shader.h"
#include "GLee.h"
#include <fstream>

// Reads a specified file into a string and returns the string.
char* ReadFile(const char* name)
{
    std::ifstream f;
    f.open(name, std::ios_base::binary); // Open
    f.seekg(0, std::ios_base::end);      // Position at end
    int length = f.tellg();              //   to get the length

    char* content = new char [length+1]; // Create buffer of needed length
    f.seekg (0, std::ios_base::beg);     // Position at beginning
    f.read (content, length);            //   to read complete file
    f.close();                           // Close

    content[length] = char(0);           // Finish with a NULL
    return content;
}

void ShaderProgram::CreateProgram()
{ 
    program = glCreateProgram();
}

void ShaderProgram::Use()
{
    glUseProgram(program);
}

void ShaderProgram::Unuse()
{
    glUseProgram(0);
}

void ShaderProgram::CreateShader(const char* fileName, int type)
{
    // Read the source from the named file
    char* src = ReadFile(fileName);
    const char* psrc[1] = {src};

    // Create a shader and attach, hand it the source, and compile it.
    int shader = glCreateShader(type);
    glAttachShader(program, shader);
    glShaderSource(shader, 1, psrc, NULL);
    glCompileShader(shader);
    delete src;

    // Get the compilation status
    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    
    // If compilation status is not OK, get and print the log message.
    if (status != 1) {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char* buffer = new char[length];
        glGetShaderInfoLog(shader, length, NULL, buffer);
        printf("Compile log for %s:\n%s\n", fileName, buffer);
        delete buffer;
    }
}

void ShaderProgram::LinkProgram()
{
    // Link program and check the status
    glLinkProgram(program);
    int status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    
    // If link failed, get and print log
    if (status != 1) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* buffer = new char[length];
        glGetProgramInfoLog(program, length, NULL, buffer);
        printf("Link log:\n%s\n", buffer);
        delete buffer;
    }
}

// Will load a texture into a texture unit and make it known to a sampler in the shader.
void ShaderProgram::ActivateTexture(const int unit, const char* name, const int texture)
{
    glActiveTexture(GL_TEXTURE0+unit);
    glBindTexture(GL_TEXTURE_2D, texture);   
    int loc = glGetUniformLocation(program, name);
    glUniform1i(loc, unit);

}
