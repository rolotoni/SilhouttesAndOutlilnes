///////////////////////////////////////////////////////////////////////
// Pass 1 fragment shader
//
// Gary Herron
//
// Copyright © 2011 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

// The pass 1 fragment shader.  This is invoked when drawing the scene
// from the viewpoint of the light.  

// The input should be a varying variable contining the coordinates of
// a pixel in the light's coordinates system.

// The output should be the shadowTexture, which is expected
// to contain the depth of light penetration into the scene.  This can
// be written into either gl_FragColor or gl_FragData[0].

uniform mat4 viewMatrix;
varying vec3 normal;
varying vec4 view_position,worldPos;

void main()
{
  gl_FragData[0].xyz = normalize(normal);
  gl_FragData[0].w = length(view_position);
  //place depth in same texture as world pos
  gl_FragData[1].xyz = worldPos.xyz;
  //float depth = length(viewMatrix*vec4(worldPos.xyz,1.0));
  //gl_FragData[1].w = vec3(depth,depth,depth);

  gl_FragData[2].xyz = gl_FrontMaterial.diffuse;
}
