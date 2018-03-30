#vs

attribute vec3 vpos, vnml;
attribute vec2 vtexcoord;

varying vec3 pos, nml;
varying vec2 uv;

uniform mat4 World, WorldInvTrans, WorldViewProj;

void main()
{
	gl_Position = WorldViewProj * vec4(vpos, 1.0);
	pos = (World * vec4(vpos, 1.0)).xyz;
	nml = normalize((WorldInvTrans * vec4(vnml, 1.0)).xyz); // Rotate normal vector by world rotation
	uv = vtexcoord;
}

#fs

varying vec3 pos, nml;
varying vec2 uv;

uniform vec3 Ambient, Diffuse, Emissive;
uniform vec4 Specular;
uniform vec3 viewpos;
uniform int numlights, lighttype[16];
uniform vec3 lightvec[16], lightclr[16];
uniform int highlighttype, HasTexture;
uniform sampler2D Texture;

void main()
{
	vec3 worldpos = pos;
	vec3 worldnml = normalize(nml);

	/*vec3 eyedir = normalize(worldpos - viewpos.xyz); // Direction from view position to world position of fragment

	// Make sure normals don't face away from the camera (to simulate normals in both directions of the triangle)
	if(dot(worldnml, eyedir) > 0.0)
		worldnml = -worldnml;

	vec3 diffuse = vec3(0.0, 0.0, 0.0), specular = vec3(0.0, 0.0, 0.0);
	for(int i = 0; i < 16; i++)
	{
		if(i >= numlights) // for(int i = 0; i < numlights; i++) doesn't work on some compilers
			break;

		vec3 lightdir = lighttype[i] == 1 ? normalize(worldpos - lightvec[i]) : lightvec[i];
		vec3 halfvec = normalize(eyedir + lightdir); // Half angle vector = vector between eyedir and lightdir

		diffuse += Diffuse * lightclr[i] * max(0.0, -dot(worldnml, lightdir));
		specular += Specular.rgb * lightclr[i] * pow(max(0.0, -dot(worldnml, halfvec)), Specular.w);
	}*/

vec3 diffuse = Diffuse, specular = vec3(0.0, 0.0, 0.0);

	if(HasTexture != 0)
		gl_FragColor = vec4(texture2D(Texture, uv).rgb * (Ambient + diffuse + Emissive) + specular, 1.0);
	else
		gl_FragColor = vec4(Ambient + diffuse + Emissive + specular, 1.0);
	/*if(highlighttype == 1)
	{
		gl_FragColor.r += 0.3;
		gl_FragColor.gb -= max(0.0, gl_FragColor.r - 1.0);
	}
	else if(highlighttype == 2)
	{
		gl_FragColor.rb += 0.3;
		gl_FragColor.g -= max(0.0, gl_FragColor.r + gl_FragColor.b - 1.0);
	}*/

// Debugging
//gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
//gl_FragColor = vec4(worldnml * 0.5 + 0.5, 1.0);
//gl_FragColor = vec4(lightdir * 0.5 + 0.5, 1.0);
//gl_FragColor = vec4(dot(eyedir, lightdir), dot(eyedir, lightdir), dot(eyedir, lightdir), 1.0);
//gl_FragColor = vec4(-dot(worldnml, lightdir), -dot(worldnml, lightdir), -dot(worldnml, lightdir), 1.0);
//gl_FragColor = vec4(-dot(worldnml, halfvec), -dot(worldnml, halfvec), -dot(worldnml, halfvec), 1.0);
//gl_FragColor = vec4(-eyedir, 1.0);
//gl_FragColor = vec4(-halfvec, 1.0);
//gl_FragColor = vec4(-eyedir.y, -eyedir.y, -eyedir.y, 1.0);
}