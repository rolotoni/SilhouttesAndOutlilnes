varying vec3 hatchWeight1;	// weight for hatch tecture unit 0, 1, 2
varying vec3 hatchWeight2;  // weight for hatch texture unit 3, 4, 5
varying vec2 texCoord0;
varying vec2 texCoord1;
varying vec2 texCoord2;
varying vec2 texCoord3;

varying float hatchLevel;

uniform sampler2D hatch1;

uniform sampler2D colorMap;
uniform float width;
uniform float height;

void main(void)
{	
    vec4 color = vec4(1.0);

	if (!(hatchWeight1 == vec3(1.0) && hatchWeight2 == vec3(1.0)))
	{
	  float intensity = hatchLevel;
	  float q = 4.0;
	  vec4 stroke = tex2D(hatch1,texCoord0);
	  color *= (intensity < 0.75 + stroke.a/q) ? stroke : vec4(1.0);
	  stroke = tex2D(hatch1,texCoord1);
	  color *= (intensity < 0.50 + stroke.a/q) ? stroke : vec4(1.0);
      stroke = tex2D(hatch1,texCoord2);
	  color *= (intensity < 0.25 + stroke.a/q) ? stroke : vec4(1.0);
	  stroke = tex2D(hatch1,texCoord3);
	  color *= (intensity < stroke.a/q) ? stroke : vec4(1.0);
	}

	gl_FragColor = vec4(color.rgb,1.0);


}