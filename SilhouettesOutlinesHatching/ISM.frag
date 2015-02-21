///////////////////////////////////////////////////////////////////////
// ISM fragment shader
//
// Gary Herron
//
// Copyright © 2011 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

varying float depth;
varying float Length;

void main()
{
	if(depth > 0.01)
		gl_FragColor = vec4(Length, Length, Length, 1.0);
	else
		discard;
}
