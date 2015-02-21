///////////////////////////////////////////////////////////////////////
// Pass 1 vertex shader
//
// Gary Herron
//
// Copyright © 2011 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

// The pass 1 vertex shader.  This is invoked when drawing the scene
// from the viewpoint of the light.  

// The input is (as always) the vertex in modeling coordinate system,
// and the modelview and perspective transformations.

// The output is (as always) the vertex transformed via both
// transformations, AND the same transformed vertex pushed through a
// varying variable to the fragment shader.

uniform mat4 viewMatrix;
varying vec3 normal;
varying vec4 view_position,worldPos;

void main()
{
    worldPos = gl_ModelViewMatrix*gl_Vertex;
    gl_Position = gl_ProjectionMatrix*viewMatrix*gl_ModelViewMatrix*gl_Vertex;
	normal = gl_NormalMatrix*gl_Normal;
	view_position = viewMatrix * worldPos;
}
