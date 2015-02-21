///////////////////////////////////////////////////////////////////////
// Pass 3 fragment shader
//
// Gary Herron
//
// Copyright © 2011 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

uniform int mode;           // Set by the application to an integer 0-9.
uniform int WIDTH;
uniform int HEIGHT;
uniform sampler2D gBuffer0;
uniform sampler2D gBuffer1;
uniform sampler2D gBuffer2;
uniform sampler2D gBuffer3;
uniform sampler2D sBuffer0;
uniform sampler2D sBuffer1;
uniform sampler2D sBuffer2;
uniform sampler2D ISM;
uniform sampler2D Filter;
uniform sampler2D FilterDepth;
uniform sampler2D FilterNormal;
uniform sampler2D Hatch;

uniform vec4 AmbienceColor;
uniform vec3 CameraPosition;
uniform vec3 LightPosition;

uniform mat4 lightViewMatrix;
uniform mat4 lightProjectionMatrix;

uniform int NumVPLs;

vec2 uv = gl_FragCoord.xy/vec2(WIDTH,HEIGHT);


vec2 GetISM_UV(vec3 w, vec3 n, vec3 p, int i, int j)
{
	vec3 pos = w - p;
	vec3 a;
	float denom = length(n.xy);
	if(denom != 0)
		a = vec3(-n.y, n.x, 0.0)/denom;
	else
		a = vec3(1.0, 0.0, 0.0);
	vec3 b = vec3(a.y*n.z, -a.x*n.z, a.y*n.x - a.x*n.y);
	pos = vec3(dot(a, pos), dot(b, pos), dot(n, pos));

	float length = length(pos);

	//normalize to get the direction
	normalize(pos);

	//project the direction onto the porabola
	vec2 uv;
	uv.x = pos.x / ( pos.z + length);
	uv.y = pos.y / ( pos.z + length);

	//move the map to the right section
	uv /= NumVPLs;
	uv += vec2((2.0*i + 1.0)/NumVPLs - 1, (2.0*j + 1.0)/NumVPLs - 1);

	uv.x *= 0.5;
	uv.y *= 0.5;
	uv.x += 0.5;
	uv.y += 0.5;

	return uv;
}

void main()
{
	if (mode == 1) 
	{
	  gl_FragColor = abs(texture2D(sBuffer0, uv));

	}
	else if (mode == 2) 
	{
	  //gl_FragColor = abs(texture2D(sBuffer1, uv))/2;
	  vec4 hatch       = texture2D(Hatch, uv);
	  gl_FragColor.xyz = hatch.xyz;
	}
	else if (mode == 3) 
	{
	  //gl_FragColor = abs(texture2D(sBuffer2, uv));
	  gl_FragColor = abs(texture2D(FilterDepth, uv));
	}
	else if (mode == 4) 
	{
      //float depth = texture2D(sBuffer0, uv).a/2.0;
	  //gl_FragColor = vec4(depth, depth, depth, 0.0);
	  gl_FragColor = abs(texture2D(FilterNormal, uv));
	}
	else if (mode == 5) 
	{
	  gl_FragColor = abs(texture2D(Filter, uv));
	}
	//else if (mode == 6) 
	//{
		//vec2 uv = gl_FragCoord.xy/vec2(WIDTH,HEIGHT);
		//gl_FragColor = abs(texture2D(gBuffer1, uv));
	//}
	else if (mode == 7) 
	{
	  gl_FragColor = abs(texture2D(gBuffer2, uv));
	}
	else if (mode == 8) 
	{
	  gl_FragColor = abs(texture2D(gBuffer3, uv));
	}
	else if (mode == 9) 
	{
      gl_FragColor = texture2D(ISM, uv)/2.0;
	}
	else
	{
		vec3 normal      = texture2D(gBuffer0, uv).xyz;
		vec3 position    = texture2D(gBuffer1, uv).xyz;
		vec4 diffuse     = texture2D(gBuffer2, uv);
		vec4 specular    = texture2D(gBuffer3, uv);
		vec4 filter      = texture2D(Filter, uv);
		vec4 hatch       = texture2D(Hatch, uv);
	  

		vec4 shadow_coord = vec4(position, 1.0);
		shadow_coord = lightProjectionMatrix*lightViewMatrix*vec4(position, 1.0);
		shadow_coord /= shadow_coord.w;
		shadow_coord.x *= 0.5;
		shadow_coord.y *= 0.5;
		shadow_coord.x += 0.5;
		shadow_coord.y += 0.5;

		vec4 light_color =  gl_LightSource[0].diffuse;

		vec3 LightVec    = normalize(LightPosition - position);

		vec3 CameraVec = normalize(position - CameraPosition);

		float light_length = length(position - LightPosition);
		float shadow_depth = texture2D(sBuffer0, shadow_coord.xy).a;

		//direct illumniation

		//ambience is nothing!
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);

		float shadow = 0.0f;
		if(light_length - 0.001 <= shadow_depth ||
		   shadow_coord.x <= 0.01 || shadow_coord.x >= 0.99 ||
		   shadow_coord.y <= 0.01 || shadow_coord.y >= 0.99)
		   shadow = 1.0f;

		//diffuse
		gl_FragColor += shadow * light_color * diffuse * max(dot(normal, LightVec), 0);

		//specular
		vec3 reflection = 2.0*normal * dot(normal, LightVec) - LightVec;
		gl_FragColor += shadow * light_color * specular * max(pow(dot(reflection, CameraVec), specular.a), 0);

		// add the light of all the fpls
		for(int i = 0; i < NumVPLs; i++)
		{
			for(int j = 0; j < NumVPLs; j++)
			{
				vec2 c = vec2((i+0.5)/NumVPLs, (j+0.5)/NumVPLs);
				vec3 vpl_position    = texture2D(sBuffer1, c).xyz;
				vec3 vpl_normal      = texture2D(sBuffer0, c).xyz;
				vec4 vpl_diffuse     = texture2D(sBuffer2, c);

				vec2 uv = GetISM_UV(position, vpl_normal, vpl_position, i, j);

				vec4 vpl_light = vpl_diffuse * max(dot(vpl_normal, normalize(LightPosition - vpl_position)), 0);

				float length = length(vpl_position - position);
				float attenuation = 2.0/(length*length + 1);

				float shadow = 1;
				if(length > texture2D(ISM, uv).x + 0.5 || dot(vpl_position-position, vpl_normal) > 0)
					shadow = 0;

				gl_FragColor += diffuse* vpl_light * shadow * attenuation/NumVPLs * max(dot(normal, (vpl_position - position)/length), 0);
			}
		}

		if(mode == 6)
		  gl_FragColor.xyz -= filter.xyz;
	}
}