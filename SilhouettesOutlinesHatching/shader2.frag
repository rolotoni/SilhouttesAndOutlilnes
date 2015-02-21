///////////////////////////////////////////////////////////////////////
// Pass 2 fragment shader
//
// Gary Herron
//
// Copyright © 2011 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

// Pass 2 fragment shader.  Invoked when drawing the scene from the
// eye's point of view. 

uniform mat4 viewMatrix;
uniform vec3 CamPos;
varying vec3 normalVec, lightVec, worldPos;

void main()
{
  gl_FragData[0].xyz = normalize(normalVec);
  gl_FragData[1].xyz = worldPos;
  float depth = length(viewMatrix*vec4(worldPos.xyz,1.0));
  float  l = length(CamPos);
  gl_FragData[1].w = depth;
  gl_FragData[1].w -= l; 
  gl_FragData[1].w += 0.5;
  gl_FragData[2].xyz = gl_FrontMaterial.diffuse;
  gl_FragData[3].xyz = gl_FrontMaterial.specular;
  gl_FragData[3].w   = gl_FrontMaterial.shininess;
  
}
