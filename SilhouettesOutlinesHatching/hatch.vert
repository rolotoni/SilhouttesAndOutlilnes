varying vec3 hatchWeight1;	// weight for hatch tecture unit 0, 1, 2
varying vec3 hatchWeight2;  // weight for hatch texture unit 3, 4, 5
varying vec2 texCoord0;
varying vec2 texCoord1;
varying vec2 texCoord2;
varying vec2 texCoord3;
varying float hatchLevel;
uniform mat4 viewMatrix;


mat4 rot90 = mat4( cos(90),-sin(90),0.0,0.0,
                      sin(90), cos(90),0.0,0.0,
					  0.0,         0.0,1.0,0.0,
					  0.0,         0.0,0.0,1.0);

mat4 rot180 = mat4( cos(180),-sin(180),0.0,0.0,
                      sin(180), cos(180),0.0,0.0,
					  0.0,         0.0,1.0,0.0,
					  0.0,         0.0,0.0,1.0);

void main()
{
    vec4 offset0 = gl_MultiTexCoord0*rot90;
	vec4 offset1 = gl_MultiTexCoord0*rot180;
    texCoord0 = vec2(gl_MultiTexCoord0);
	texCoord0.s = texCoord0.s * 3.0;
    texCoord0.t = texCoord0.t * 1.5;
	texCoord1 = texCoord0 + offset0.xy;
	texCoord2 = texCoord0.yx + offset0.zw;
	texCoord3 = texCoord0.yx + offset1.xy;

	
    vec3 lightVec;


	gl_Position = gl_ProjectionMatrix*viewMatrix*gl_ModelViewMatrix*gl_Vertex;
	
	vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
	vec4 vertex = gl_ModelViewMatrix * gl_Vertex;
	lightVec = (gl_ModelViewMatrix*gl_LightSource[0].position - vertex).xyz;
	vec3 light = normalize(gl_LightSource[0].diffuse - lightVec).xyz;
	
	
	//float diffuseValue = gl_LightSource[0].diffuse * max(dot(normal, lightVec), 0);
	float diffuseValue = max(dot(normal, light), 0.0);
	
	hatchLevel = diffuseValue * 1.0;
	
	hatchWeight1 = vec3(0.0);
	hatchWeight2 = vec3(0.0);
	
	if (hatchLevel >= 5.0)
	{
		hatchWeight1 = vec3(1.0);
		hatchWeight2 = vec3(1.0);
		//hatchWeight1.x = 1.0 - (4.0 - hatchLevel);
		//hatchWeight1.y = 1.0 - hatchWeight1.x;
	}
	else if (hatchLevel >= 4.0)
	{
		hatchWeight1.y = 1.0 - (3.0 - hatchLevel);
		hatchWeight1.z = 1.0 - hatchWeight1.y;
	}
	else if (hatchLevel >= 3.0)
	{
		hatchWeight1.z = 1.0 - (2.0 - hatchLevel);
		hatchWeight2.x = 1.0 - hatchWeight1.z;
	}
	else if (hatchLevel >= 2.0)
	{
		hatchWeight2.x = 1.0 - (1.0 - hatchLevel);
		hatchWeight2.y = 1.0 - hatchWeight2.x;
	}
	else
	{
		hatchWeight2.y = 1.0 - (0.0 - hatchLevel);
		hatchWeight2.z = 1.0 - hatchWeight1.y;
	}	
}
