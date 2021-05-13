// vertex attributes
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord;

// matrices
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform int before_game;


out vec4 epos;	// eye-space position
out vec3 norm;	// per-vertex normal before interpolation
out vec2 tc;	// texture coordinate

void main()
{
	if(before_game==0) {
		gl_Position = vec4(position,1);
		tc = texcoord;
		norm = vec3(0);
		return;
	}
	vec4 wpos = model_matrix * vec4(position,1);
	vec4 epos = view_matrix * wpos;
	
	gl_Position = projection_matrix * epos;
	//gl_Position=vec4(position,1);
	norm = normalize(mat3(view_matrix*model_matrix)*normal);
	// pass texture coordinate to fragment shader
	tc = texcoord;
}