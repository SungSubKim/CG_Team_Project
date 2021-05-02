#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// input from vertex shader
in vec4 epos;
in vec3 norm;
in vec2 tc;

// the only output variable
out vec4 fragColor;

// texture sampler
uniform mat4	view_matrix;
uniform sampler2D TEX_SKY;
//uniform sampler2D TEX_MAP1;
//uniform sampler2D TEX_MAP2;
//uniform sampler2D TEX_MAP3;

uniform bool use_texture;
uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform vec4 emissive;

uniform float	shininess;
uniform vec4	light_position, Ia, Id, Is;	// light
uniform vec4	Ka, Kd, Ks;					// material properties

vec4 phong( vec3 l, vec3 n, vec3 h, vec4 Kd )
{
	vec4 Ira = Ka*Ia;									// ambient reflection
	vec4 Ird = max(Kd*dot(l,n)*Id,0.0);					// diffuse reflection
	vec4 Irs = max(Ks*pow(dot(h,n),shininess)*Is,0.0);	// specular reflection
	return Ira + Ird + Irs;
}

void main()
{
	vec4 lpos = view_matrix*light_position;
	vec3 n = normalize(norm);	// norm interpolated via rasterizer should be normalized again here
	vec3 p = epos.xyz;			// 3D position of this fragment
	vec3 l = normalize(lpos.xyz-(lpos.a==0.0?vec3(0):p));	// lpos.a==0 means directional light
	vec3 v = normalize(-p);		// eye-epos = vec3(0)-epos
	vec3 h = normalize(l+v);	// the halfway vector
	vec4 iKd=texture(TEX_SKY,tc);
	fragColor = phong( l, n, h, iKd );
	fragColor=texture(TEX_SKY,tc);
	
	
}