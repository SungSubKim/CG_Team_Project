#include "cgmath.h"			// slee's simple math library
#define STB_IMAGE_IMPLEMENTATION
#include "cgut2.h"			// slee's OpenGL utility
#include "trackball.h"
#include "assimp_loader.h"
//#include "circle.h"

//*************************************
// global constants
static const char*	window_name = "cgmodel - assimp for loading {obj|3ds} files";
static const char*	vert_shader_path = "../bin/shaders/model.vert";
static const char*	frag_shader_path = "../bin/shaders/model.frag";
static const char* mesh_obj = "../bin/mesh/Map2_black.obj";
static const char* sphere_obj = "../bin/mesh/tmpcharacter.obj";

std::vector<vertex>	unit_circle_vertices;	// host-side vertices
//*************************************
// common structures
struct camera
{
	vec3	eye = vec3(0, 50, 120 );
	vec3	at = vec3( -2, 0, 0 );
	vec3	up = vec3( 0, 1, 0 );
	mat4	view_matrix = mat4::look_at( eye, at, up );
		
	float	fovy = PI/4.0f; // must be in radian
	float	aspect_ratio;
	float	dNear = 1.0f;
	float	dFar = 1000.0f;
	mat4	projection_matrix;
};

struct light_t
{
	vec4	position = vec4(1.0f, 1.0f, 1.0f, 0.0f);   // directional light
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct material_t
{
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	float	shininess = 100000.0f;
};


//auto	circles = std::move(create_circles());
vec3 s_center = vec3(-62, 22 ,-35); // sphere의 시작지점
mat4 model_matrix_sphere; //sphere를 조종하는 model matrix
float scale = 1.0f;
mat4 model_matrix_map = mat4::scale(scale, scale, scale); //맵의 모델 매트릭스
GLuint vertex_buffer = 0;	// ID holder for vertex buffer
GLuint index_buffer = 0;		// ID holder for index buffer
GLuint	vertex_array = 0;	// ID holder for vertex array object*************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2(1280, 720); // cg_default_window_size(); // initial window size

//*************************************
// OpenGL objects
GLuint	program	= 0;	// ID holder for GPU program

//*************************************
// global variables
int		frame = 0;		// index of rendering frames
bool	show_texcoord = false;
bool	b_wireframe = false;
//*************************************
// scene objects
mesh2* pMesh = nullptr, * sMesh = nullptr;
camera		cam;
trackball	tb;
bool l = false, r = false, u = false, d = false;
float old_t=0;
mat4 model_matrix0;
light_t		light;
material_t	material;
//*************************************
float min(float a, float b) {
	return a < b ? a : b;
}
void update()
{
	glUseProgram(program);

	// update projection matrix
	cam.aspect_ratio = window_size.x/float(window_size.y);
	cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect_ratio, cam.dNear, cam.dFar );

	// build the model matrix for oscillating scale
	float t = float(glfwGetTime());
	if (l) {
		s_center.x -= (t - old_t) * 50;
		//printf("%f\n",circles[0].center.y);
	}
	else if (r)
		s_center.x += (t - old_t) * 50;
	else if (u)
		s_center.z -= (t - old_t) * 50;
	else if (d)
		s_center.z += (t - old_t) * 50;
	old_t = t;

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );

	glUniform4fv(glGetUniformLocation(program, "light_position"), 1, light.position);
	glUniform4fv(glGetUniformLocation(program, "Ia"), 1, light.ambient);
	glUniform4fv(glGetUniformLocation(program, "Id"), 1, light.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Is"), 1, light.specular);

	glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);

	glActiveTexture(GL_TEXTURE0);								// select the texture slot to bind
}
float old_t2 = 0;
void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// notify GL that we use our own program
	glUseProgram( program );

	// bind vertex array object
	glBindVertexArray( pMesh->vertex_array );

	GLint uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_map);
	for( size_t k=0, kn=pMesh->geometry_list.size(); k < kn; k++ ) {
		geometry& g = pMesh->geometry_list[k];
		
		//printf("%lf\n", g.mat->textures.ambient.id);
		if (g.mat->textures.diffuse) {
			glBindTexture(GL_TEXTURE_2D, g.mat->textures.diffuse->id);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);	 // GL_TEXTURE0
			glUniform1i(glGetUniformLocation(program, "use_texture"), true);
		} else {
			glUniform4fv(glGetUniformLocation(program, "ambient"), 1, (const float*)(&g.mat->ambient));
			glUniform4fv(glGetUniformLocation(program, "diffuse"), 1, (const float*)(&g.mat->diffuse));
			glUniform4fv(glGetUniformLocation(program, "specular"), 1, (const float*)(&g.mat->specular));
			glUniform4fv(glGetUniformLocation(program, "emissive"), 1, (const float*)(&g.mat->emissive));
			//glUniform4fv(glGetUniformLocation(program, "diffuse"), 1, (const float*)(&g.mat->diffuse));
			glUniform1i(glGetUniformLocation(program, "use_texture"), false);
		}
		

		// render vertices: trigger shader programs to process vertex data
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pMesh->index_buffer );
		GLint uloc;
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
		glDrawElements( GL_TRIANGLES, g.index_count, GL_UNSIGNED_INT, (GLvoid*)(g.index_start*sizeof(GLuint)) );
	}

	// swap front and back buffers, and display to screen
	//glfwSwapBuffers( window );
	// bind vertex array object
	glBindVertexArray(sMesh->vertex_array);
	model_matrix_sphere = mat4::translate(s_center) *mat4::scale(vec3(0.2f))  ;// s_center의 정보를 반영
	for (size_t k = 0, kn = sMesh->geometry_list.size(); k < kn; k++) {
		geometry& g = sMesh->geometry_list[k];
		/*g.mat->ambient.a = 0;
		g.mat->diffuse.a = 0;
		g.mat->specular.a = 0;
		g.mat->emissive.a = 0;*/
		//g.mat->diffuse.x = 0.0f;
		//g.mat->diffuse.a = 0.3f;
		if (g.mat->textures.diffuse) {
			printf("asdf\n");
			glBindTexture(GL_TEXTURE_2D, g.mat->textures.diffuse->id);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);	 // GL_TEXTURE0
			glUniform1i(glGetUniformLocation(program, "use_texture"), true);
			
		}
		else {
			glUniform4fv(glGetUniformLocation(program, "ambient"), 1, (const float*)(&g.mat->ambient));
			glUniform4fv(glGetUniformLocation(program, "diffuse"), 1, (const float*)(&g.mat->diffuse));
			glUniform4fv(glGetUniformLocation(program, "specular"), 1, (const float*)(&g.mat->specular));
			glUniform4fv(glGetUniformLocation(program, "emissive"), 1, (const float*)(&g.mat->emissive));
			glUniform1i(glGetUniformLocation(program, "use_texture"), false);
			//printf("%lf \n", g.mat->diffuse.a);
		}
		//sMesh.
		// render vertices: trigger shader programs to process vertex data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sMesh->index_buffer);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_sphere);
		//model_matrix를 uniform으로 넘겨주기
		glDrawElements(GL_TRIANGLES, g.index_count, GL_UNSIGNED_INT, (GLvoid*)(g.index_start * sizeof(GLuint)));
	}

	glfwSwapBuffers(window);
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
	printf( "- press Home to reset camera\n" );
	printf( "- press 'd' to toggle between OBJ format and 3DS format\n" );
	printf( "\n" );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_HOME)					cam = camera();
		else if (key == GLFW_KEY_T)					show_texcoord = !show_texcoord;
		else if (key == GLFW_KEY_D)
		{
			static bool is_obj = true;
			is_obj = !is_obj;

			glFinish();
			delete_texture_cache();
			delete pMesh;
			pMesh = load_model(mesh_obj);
		}
#ifndef GL_ES_VERSION_2_0
		else if (key == GLFW_KEY_W)
		{
			b_wireframe = !b_wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, b_wireframe ? GL_LINE : GL_FILL);
			printf("> using %s mode\n", b_wireframe ? "wireframe" : "solid");
		}
		else if (key == GLFW_KEY_LEFT) {
			l = true;
			old_t2 = (float)glfwGetTime();
		}
			
		else if (key == GLFW_KEY_RIGHT)
			r = true;
		else if (key == GLFW_KEY_UP)
			u = true;
		else if (key == GLFW_KEY_DOWN)
			d = true;
#endif
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT)
			l = false;
		else if (key == GLFW_KEY_RIGHT)
			r = false;
		else if (key == GLFW_KEY_UP)
			u = false;
		else if (key == GLFW_KEY_DOWN)
			d = false;
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if(button==GLFW_MOUSE_BUTTON_LEFT)
	{
		dvec2 pos; glfwGetCursorPos(window,&pos.x,&pos.y);
		vec2 npos = cursor_to_ndc( pos, window_size );
		if(action==GLFW_PRESS)			tb.begin( cam.view_matrix, npos );
		else if(action==GLFW_RELEASE)	tb.end();
	}
}

void motion( GLFWwindow* window, double x, double y )
{
	if(!tb.is_tracking()) return;
	vec2 npos = cursor_to_ndc( dvec2(x,y), window_size );
	cam.view_matrix = tb.update( npos );
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor( 0/255.0f, 0/255.0f, 0/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	glEnable( GL_TEXTURE_2D );
	glActiveTexture( GL_TEXTURE0 );

	// load the mesh
	pMesh = load_model(mesh_obj);
	if(pMesh==nullptr){ printf( "Unable to load mesh\n" ); return false; }
	sMesh = load_model(sphere_obj);
	if (pMesh == nullptr) { printf("Unable to load mesh\n"); return false; }

	return true;
}

void user_finalize()
{
	delete_texture_cache();
	delete pMesh;
}

int main( int argc, char* argv[] )
{
	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}
	
	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}

