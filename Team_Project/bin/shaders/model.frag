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
uniform bool	sky;
uniform bool	snow;
uniform int		mode;
//sky변수의 추가 이것에 따라 어떤 값을 fragColor로 사용할지 결정
uniform mat4	view_matrix;
uniform sampler2D TEX_SKY;
uniform sampler2D TEX_SNOW;
//uniform sampler2D TEX_MAP1;
//uniform sampler2D TEX_MAP2;
//uniform sampler2D TEX_MAP3;
uniform sampler2D SKY_LEFT;
uniform sampler2D SKY_DOWN;
uniform sampler2D SKY_BACK;
uniform sampler2D SKY_RIGHT;
uniform sampler2D SKY_UP;
uniform sampler2D SKY_FRONT;

uniform bool use_texture;
uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform vec4 emissive;
uniform vec4 color;

uniform float	shininess;
uniform vec4	light_position, Ia, Id, Is;	// light
uniform vec4	Ka, Kd, Ks;					// material properties

vec4 phong( vec3 l, vec3 n, vec3 h, vec4 Kd )
{
	vec4 Ira = ambient*Ia;									// ambient reflection
	vec4 Ird = max(diffuse*dot(l,n)*Id,0.0);					// diffuse reflection
	vec4 Irs = max(specular*pow(dot(h,n),shininess)*Is,0.0);	// specular reflection
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
	vec4 iKd = texture(SKY_LEFT,tc);
	
	if(sky){
		if(mode==1){
			fragColor=texture(SKY_LEFT,tc);
		}else if(mode==2){
			fragColor=texture(SKY_DOWN,tc);
		}else if(mode==3){
			fragColor=texture(SKY_BACK,tc);
		}else if(mode==4){
			fragColor=texture(SKY_RIGHT,tc);
		}else if(mode==5){
			fragColor=texture(SKY_UP,tc);
		}else if(mode==6){
			fragColor=texture(SKY_FRONT,tc);
		}
	}else if(snow) {
		fragColor = texture( TEX_SNOW, tc );
		if(fragColor.a < 0.001) 
			discard;
		fragColor = vec4(fragColor.rgb,fragColor.r) * color; // enable alpha blending
	}
	else fragColor = phong( l, n, h, iKd );
	
	
	
	
	// TEX_SKY와 phong중 어느걸 사용할지를 sky에 따라 결정
}