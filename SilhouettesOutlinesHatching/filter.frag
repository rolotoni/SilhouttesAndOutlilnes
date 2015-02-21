///////////////////////////////////////////////////////////////////////
// image filter fragment shader
//
// Jesse Harrison
////////////////////////////////////////////////////////////////////////

#define KERNEL_SIZE 9
float kernel[KERNEL_SIZE];

uniform sampler2D Normal;
uniform sampler2D WorldPos;
uniform sampler2D colorMap;
uniform float width;
uniform float height;

float step_w = 1.0/width;
float step_h = 1.0/height;

vec2 uv = gl_FragCoord.xy;

vec2 offset[KERNEL_SIZE];
	
uniform mat3 G[2] = mat3[]
(
	mat3( 1.0, 2.0, 1.0, 0.0, 0.0, 0.0, -1.0, -2.0, -1.0 ),
	mat3( 1.0, 0.0, -1.0, 2.0, 0.0, -2.0, 1.0, 0.0, -1.0 )
);
		
vec4 DepthGrab(void)
{
  vec3 tc = vec3(1.0, 0.0, 0.0);
  mat3 I;
  float cnv[2];
  vec3 sample;
  
  float dx = 1.0/width;
  float dy = 1.0/height;

  // fetch the 3x3 neighbourhood and use the RGB vector's length as intensity value
  for (float i=0; i<3; i++)
  {
    for (float j=0; j<3; j++) 
    {
      sample.rgb = texelFetch(WorldPos, ivec2(uv) + ivec2(i-1,j-1), 0 ).w;
      I[i][j] = length(sample); 
    }
  }
  
  // calculate the convolution values for all the masks
  for (int i=0; i<2; i++) 
  {
    float dp3 = dot(G[i][0], I[0]) + dot(G[i][1], I[1]) + dot(G[i][2], I[2]);
    cnv[i] = dp3 * dp3; 
  }
  
  return vec4(0.5 * sqrt(cnv[0]*cnv[0]+cnv[1]*cnv[1]));
}			

vec4 NormalGrab(void)
{
  vec3 tc = vec3(1.0, 0.0, 0.0);
  mat3 I;
  float cnv[2];
  vec4 sample, red, green, blue;
  
  // fetch the 3x3 neighbourhood and use the RGB vector's length as intensity value
  for (int i=0; i<3; i++)
  {
    for (int j=0; j<3; j++) 
    {
      sample.r = texelFetch(Normal, ivec2(uv) + ivec2(i-1,j-1), 0).r;
	  sample.gb = 0.0;
      I[i][j] = length(sample); 
    }
  }
  
  // calculate the convolution values for all the masks
  for (int i=0; i<2; i++) 
  {
    float dp3 = dot(G[i][0], I[0]) + dot(G[i][1], I[1]) + dot(G[i][2], I[2]);
    cnv[i] = dp3 * dp3; 
  }
  
  red = vec4(0.5 * sqrt(cnv[0]*cnv[0]+cnv[1]*cnv[1]));

  for (int i=0; i<3; i++)
  {
    for (int j=0; j<3; j++) 
    {
      sample.g = texelFetch(Normal, ivec2(uv)+ ivec2(i-1,j-1), 0).g;
	  sample.rb = 0.0;
      I[i][j] = length(sample); 
    }
  }
  
  // calculate the convolution values for all the masks
  for (int i=0; i<2; i++) 
  {
    float dp3 = dot(G[i][0], I[0]) + dot(G[i][1], I[1]) + dot(G[i][2], I[2]);
    cnv[i] = dp3 * dp3; 
  }
  
  green = vec4(0.5 * sqrt(cnv[0]*cnv[0]+cnv[1]*cnv[1]));

  for (int i=0; i<3; i++)
  {
    for (int j=0; j<3; j++) 
    {
      sample.b = texelFetch(Normal, ivec2(uv) + ivec2(i-1,j-1), 0).b;
	  sample.rg = 0.0;
      I[i][j] = length(sample); 
    }
  }
  
  // calculate the convolution values for all the masks
  for (int i=0; i<2; i++) 
  {
    float dp3 = dot(G[i][0], I[0]) + dot(G[i][1], I[1]) + dot(G[i][2], I[2]);
    cnv[i] = dp3 * dp3; 
  }
  
  blue = vec4(0.5 * sqrt(cnv[0]*cnv[0]+cnv[1]*cnv[1]));

  return vec4(clamp(red.r+green.r+blue.r,0,1),clamp(red.g+green.g+blue.g,0,1),clamp(red.b+green.b+blue.b,0,1), 1.0);
} 

void main(void)
{
  vec4 depth = DepthGrab();
  vec4 normal = NormalGrab();
  gl_FragData[0] = depth + normal;
  gl_FragData[1] = depth;
  gl_FragData[2] = normal;
}
