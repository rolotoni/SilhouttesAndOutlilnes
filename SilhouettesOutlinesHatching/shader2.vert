///////////////////////////////////////////////////////////////////////
// Pass 2 vertex shader
//
// Gary Herron
//
// Copyright © 2011 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

// Pass 2 vertex shader. 

uniform mat4 viewMatrix;
varying vec3 normalVec, lightVec, worldPos;

void main()
{
    worldPos = gl_ModelViewMatrix*gl_Vertex;
    gl_Position = gl_ProjectionMatrix*viewMatrix*gl_ModelViewMatrix*gl_Vertex;
	
	normalVec = gl_NormalMatrix*gl_Normal;	
	lightVec = (gl_ModelViewMatrix*gl_LightSource[0].position - gl_ModelViewMatrix*gl_Vertex).xyz;

}
