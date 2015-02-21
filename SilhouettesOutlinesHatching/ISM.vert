///////////////////////////////////////////////////////////////////////
// ISM vertex shader
//
// Gary Herron
//
// Copyright © 2011 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////


uniform int ISM_RES;
uniform int NUM_ISM;
uniform float near;
uniform float far;


uniform sampler2D sBuffer0;
uniform sampler2D sBuffer1;

varying float depth;
varying float Length;

uniform mat4 viewMatrix;

void main()
{
    //NUM_ISM = 1;
	vec2 Tc = vec2(gl_MultiTexCoord0.x,gl_MultiTexCoord0.y);

	vec2 c = vec2((Tc.x+0.5)/NUM_ISM, (Tc.y+0.5)/NUM_ISM);
	vec3 position = texture2D(sBuffer1, c).xyz;
	vec3 n = texture2D(sBuffer0, c).xyz;

	//get the position in world space
	
	vec4 w = gl_ModelViewMatrix*gl_Vertex;
	vec3 pos = w.xyz - position;
	vec3 a;
	float denom = length(n.xy);
	if(denom != 0.0)
		a = vec3(-n.y, n.x, 0.0)/denom;
	else
		a = vec3(1.0, 0.0, 0.0);
	vec3 b = vec3(a.y*n.z, -a.x*n.z, a.y*n.x - a.x*n.y);
	pos = vec3(dot(a, pos), dot(b, pos), dot(n, pos));

	Length = length(pos);
	depth = pos.z;

	//normalize to get the direction
	normalize(pos);

	//project the direction onto the porabola
	gl_Position.x = pos.x / (pos.z + Length);
	gl_Position.y = pos.y / (pos.z + Length);

	//move the map to the right section
	gl_Position.xy /= NUM_ISM+(.01*NUM_ISM);
	gl_Position.xy += vec2((2.0*Tc.x + 1.0)/NUM_ISM - 1, (2.0*Tc.y + 1.0)/NUM_ISM - 1);
	gl_Position.z = Length/far;
	gl_Position.w = 1.0;

	gl_PointSize = min(5.0/Length, 4.0);

	Length = length(w.xyz - position);
}
