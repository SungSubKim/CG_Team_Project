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
uniform bool	sky, snow;
uniform float	alpha;
uniform int		mode;
uniform int		model_number;
//sky변수의 추가 이것에 따라 어떤 값을 fragColor로 사용할지 결정
uniform mat4	view_matrix;
//uniform sampler2D TEX_MAP1, TEX_MAP2, TEX_MAP3;
uniform sampler2D TEX_SKY, TEX_SNOW, SKY_LEFT, SKY_DOWN ,SKY_BACK, SKY_RIGHT, SKY_UP, SKY_FRONT, TITLETEX
,HELPTEX1,HELPTEX2,HELPTEX3,FINALTEX,DIETEX;
uniform bool opacity;
uniform bool use_texture,b_help;
uniform vec4 ambient, diffuse, specular, emissive, color;

uniform float	shininess;
uniform vec4	light_position, Ia, Id, Is;	// light
uniform vec4	Ka, Kd, Ks;					// material properties

uniform int before_game, stage;
vec4 phong( vec3 l, vec3 n, vec3 h, vec4 Kd )
{
	vec4 Ira = ambient*Ia;									// ambient reflection
	vec4 Ird = max(diffuse*dot(l,n)*Id,0.0);					// diffuse reflection
	vec4 Irs = max(specular*pow(dot(h,n),shininess)*Is,0.0);	// specular reflection
	return Ira + Ird + Irs;
}

void main()
{	
	if (before_game <3) {
		fragColor = texture( TITLETEX, tc );
		return;
	}
	if (b_help) {
		switch( stage) {
			case 1:
				fragColor = texture( HELPTEX1, tc );
				return;
			case 2:
				fragColor = texture( HELPTEX2, tc );
				return;
			case 3:
				fragColor = texture( HELPTEX3, tc );
				return;
			case 4:
				fragColor = texture( FINALTEX, tc );
				return;
			case 5:
				fragColor = texture( DIETEX, tc );
				return;
			default:
				return;
		}
	}
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
	else{
		fragColor = phong( l, n, h, iKd );
		if(opacity==true&&model_number==7){
			fragColor.a=0.3f;
		}
	}
}