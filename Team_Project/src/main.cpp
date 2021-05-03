#include "cgmath.h"			// slee's simple math library
#define STB_IMAGE_IMPLEMENTATION
#include "cgut2.h"			// slee's OpenGL utility
#include "trackball.h"
#include "assimp_loader.h"
#include "model.h"
//#include "circle.h"

//*************************************
// global constants
static const char*	window_name = "Team project - Woori's Adventure";
static const char*	vert_shader_path = "../bin/shaders/model.vert";
static const char*	frag_shader_path = "../bin/shaders/model.frag";
static const char* sky_image_path = "../bin/images/skybox.jpeg"; //하늘 파일경로

std::vector<vertex>	unit_circle_vertices;	// host-side vertices
//*************************************
// common structures
struct camera
{
	vec3	eye = vec3(0 + MAP_X / 2, 100, 120 + MAP_Z / 2);
	vec3	at = vec3(-2 + MAP_X / 2, 0, 0 + MAP_Z / 2);
	vec3	up = vec3(0, 1, 0);
	mat4	view_matrix = mat4::look_at(eye, at, up);

	float	fovy = PI / 4.0f; // must be in radian
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
//vec3 s_center = vec3(-62, 22 ,-35); // sphere의 시작지점
mat4 model_matrix_sphere; //sphere를 조종하는 model matrix
float scale = 1.0f;
mat4 model_matrix_map = mat4::scale(scale, scale, scale); //맵의 모델 매트릭스
mat4 model_matrix_background=mat4::translate(0.0f,0.0f,-250.0f)*mat4::scale(500.0f,500.0f,100.0f);

// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2(1280, 720); // cg_default_window_size(); // initial window size

//*************************************
// OpenGL objects
GLuint	program	= 0;	// ID holder for GPU program
GLuint vertex_buffer = 0;	// ID holder for vertex buffer
GLuint index_buffer = 0;		// ID holder for index buffer
GLuint	vertex_array = 0;	// ID holder for vertex array object*************************
GLuint	TEX_SKY = 0;
//*************************************
// global variables
int		frame = 0;		// index of rendering frames
bool	show_texcoord = false;
bool	b_wireframe = false;
//*************************************
// scene objects
mesh2* pMesh = nullptr, * sMesh = nullptr, * bMesh=nullptr;
mesh2* mapMesh2_1 = nullptr, * mapMesh2_2 = nullptr, * mapMesh2_3 = nullptr;

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
	cam.aspect_ratio = window_size.x / float(window_size.y);
	cam.projection_matrix = mat4::perspective(cam.fovy, cam.aspect_ratio, cam.dNear, cam.dFar);

	// build the model matrix for oscillating scale
	float t = float(glfwGetTime());
	int rate = 50; if (deaccel_keys) rate /= 2;
	if (l) {
		s_center.x -= (t - old_t) * rate;
		//printf("%f\n",circles[0].center.y);
	}
	else if (r)
		s_center.x += (t - old_t) * rate;
	else if (u)
		s_center.z -= (t - old_t) * rate;
	else if (d)
		s_center.z += (t - old_t) * rate;
	old_t = t;
	check_on_area();
	model& m = getModelByName("sphere");
	m.model_matrix = mat4::translate(s_center) * mat4::scale(vec3(0.4f));

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation(program, "view_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.view_matrix);
	uloc = glGetUniformLocation(program, "projection_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.projection_matrix);

	glUniform4fv(glGetUniformLocation(program, "light_position"), 1, light.position);
	glUniform4fv(glGetUniformLocation(program, "Ia"), 1, light.ambient);
	glUniform4fv(glGetUniformLocation(program, "Id"), 1, light.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Is"), 1, light.specular);

	glUniform4fv(glGetUniformLocation(program, "Ka"), 1, material.ambient);
	glUniform4fv(glGetUniformLocation(program, "Kd"), 1, material.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Ks"), 1, material.specular);
	glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);

	glActiveTexture(GL_TEXTURE0);								// select the texture slot to bind
	glBindTexture(GL_TEXTURE_2D, TEX_SKY);
	glUniform1i(glGetUniformLocation(program, "TEX_SKY"), 0);	 // GL_TEXTURE0
									// select the texture slot to bind
}
void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// notify GL that we use our own program
	glUseProgram(program);

	GLint uloc = glGetUniformLocation(program, "sky");
	if (uloc > -1) glUniform1i(uloc, false);
	for (auto& model : models) {
		if (!model.visible)
			continue;
		mesh2* pMesh = model.pMesh;
		glBindVertexArray(pMesh->vertex_array);
		GLint uloc = glGetUniformLocation(program, "model_matrix");
		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model.model_matrix);

		for (size_t k = 0, kn = pMesh->geometry_list.size(); k < kn; k++) {
			geometry& g = pMesh->geometry_list[k];

			//printf("%lf\n", g.mat->textures.ambient.id);
			if (g.mat->textures.diffuse) {
				glBindTexture(GL_TEXTURE_2D, g.mat->textures.diffuse->id);
				glUniform1i(glGetUniformLocation(program, "TEX"), 0);	 // GL_TEXTURE0
				glUniform1i(glGetUniformLocation(program, "use_texture"), true);
			}
			else {
				glUniform4fv(glGetUniformLocation(program, "ambient"), 1, (const float*)(&g.mat->ambient));
				glUniform4fv(glGetUniformLocation(program, "diffuse"), 1, (const float*)(&g.mat->diffuse));
				glUniform4fv(glGetUniformLocation(program, "specular"), 1, (const float*)(&g.mat->specular));
				glUniform4fv(glGetUniformLocation(program, "emissive"), 1, (const float*)(&g.mat->emissive));
				//glUniform4fv(glGetUniformLocation(program, "diffuse"), 1, (const float*)(&g.mat->diffuse));
				glUniform1i(glGetUniformLocation(program, "use_texture"), false);
			}

			// render vertices: trigger shader programs to process vertex data
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->index_buffer);
			glDrawElements(GL_TRIANGLES, g.index_count, GL_UNSIGNED_INT, (GLvoid*)(g.index_start * sizeof(GLuint)));
		}
	}


	//printf("%lf\n", g.mat->textures.ambient.id);
	uloc = glGetUniformLocation(program, "sky");
	if (uloc > -1) glUniform1i(uloc, true);
	uloc = glGetUniformLocation(program, "model_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_background); //구 사용
	glActiveTexture(GL_TEXTURE0); // 배경화면 그리기.
	glBindTexture(GL_TEXTURE_2D, TEX_SKY);
	glUniform1i(glGetUniformLocation(program, "TEX_SKY"), 0);
	glBindVertexArray(vertex_array);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/*for (size_t k = 0, kn = bMesh->geometry_list.size(); k < kn; k++) {// 배경화면
		geometry& g = bMesh->geometry_list[k];

		if (g.mat->textures.diffuse) {
			printf("asdf\n");
			glBindTexture(GL_TEXTURE_2D, g.mat->textures.diffuse->id);
			glUniform1i(glGetUniformLocation(program, "TEX_SKY"), 0);	 // GL_TEXTURE0
			glUniform1i(glGetUniformLocation(program, "use_texture"), true);

		}
		else {
			glUniform4fv(glGetUniformLocation(program, "ambient"), 0, (const float*)(&g.mat->ambient));
			glUniform4fv(glGetUniformLocation(program, "diffuse"), 0, (const float*)(&g.mat->diffuse));
			glUniform4fv(glGetUniformLocation(program, "specular"), 0, (const float*)(&g.mat->specular));
			glUniform4fv(glGetUniformLocation(program, "emissive"), 0, (const float*)(&g.mat->emissive));
			glUniform1i(glGetUniformLocation(program, "use_texture"), false);
			//printf("%lf \n", g.mat->diffuse.a);
		}

		//sMesh.
		// render vertices: trigger shader programs to process vertex data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bMesh->index_buffer);

		glDrawElements(GL_TRIANGLES, g.index_count, GL_UNSIGNED_INT, (GLvoid*)(g.index_start * sizeof(GLuint)));
	}*/


	// render vertices: trigger shader programs to process vertex data
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	//GLint uloc;


	glfwSwapBuffers(window);
}

void reshape(GLFWwindow* window, int width, int height)
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width, height);
	glViewport(0, 0, width, height);
}

void print_help()
{
	printf("[help]\n");
	printf("- press ESC or 'q' to terminate the program\n");
	printf("- press F1 or 'h' to see help\n");
	printf("- press Home to reset camera\n");
	printf("- press 'd' to toggle between OBJ format and 3DS format\n");
	printf("\n");
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//printf("%d %d %d \n", action, mods, key);
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_HOME)					cam = camera();
		else if (key == GLFW_KEY_T) {
			model& m = getModelByName("triangle");
			if (&m == &none)
				printf("not found: triangle\n");
			m.visible = !m.visible;
			show_texcoord = !show_texcoord;
		}
		else if (key == GLFW_KEY_D)
		{
			static bool is_obj = true;
			is_obj = !is_obj;

			glFinish();
			delete_texture_cache();
			/*for (int i=0; i<3; i++)
				delete pMesh[i];
				pMesh = load_model(mesh_obj);*/
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
		}

		else if (key == GLFW_KEY_RIGHT) {
			r = true;
		}
		else if (key == GLFW_KEY_UP) {
			u = true;
		}
		else if (key == GLFW_KEY_DOWN) {
			d = true;
		}
		else if (key == GLFW_KEY_B) {
			model& m = getModelByName("Map2_2");
			if (&m == &none)
				printf("not found: Map2_2\n");
			m.visible = !m.visible;
		}
		else if (key == GLFW_KEY_LEFT_CONTROL)
			deaccel_keys = 1;
#endif
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) {
			l = false;
		}
		else if (key == GLFW_KEY_RIGHT) {
			r = false;
		}
		else if (key == GLFW_KEY_UP) {
			u = false;
		}
		else if (key == GLFW_KEY_DOWN) {
			d = false;
		}
		else if (key == GLFW_KEY_LEFT_CONTROL)
			deaccel_keys = 0;
	}
}

void mouse(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = cursor_to_ndc(pos, window_size);
		if (action == GLFW_PRESS)			tb.begin(cam.view_matrix, npos);
		else if (action == GLFW_RELEASE)	tb.end();
	}
}

void motion(GLFWwindow* window, double x, double y)
{
	if (!tb.is_tracking()) return;
	vec2 npos = cursor_to_ndc(dvec2(x, y), window_size);
	cam.view_matrix = tb.update(npos);
}

GLuint create_texture(const char* image_path, bool mipmap = true, GLenum wrap = GL_CLAMP_TO_EDGE, GLenum filter = GL_LINEAR)
{
	// load image
	image* i = cg_load_image(image_path); if (!i) return 0; // return null texture; 0 is reserved as a null texture
	int		w = i->width, h = i->height, c = i->channels;

	// induce internal format and format from image
	GLint	internal_format = c == 1 ? GL_R8 : c == 2 ? GL_RG8 : c == 3 ? GL_RGB8 : GL_RGBA8;
	GLenum	format = c == 1 ? GL_RED : c == 2 ? GL_RG : c == 3 ? GL_RGB : GL_RGBA;

	// create a src texture (lena texture)
	GLuint texture;
	glGenTextures(1, &texture); if (texture == 0) { printf("%s(): failed in glGenTextures()\n", __func__); return 0; }
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, GL_UNSIGNED_BYTE, i->ptr);
	if (i) { delete i; i = nullptr; } // release image

	// build mipmap
	if (mipmap)
	{
		int mip_levels = 0; for (int k = w > h ? w : h; k; k >>= 1) mip_levels++;
		for (int l = 1; l < mip_levels; l++)
			glTexImage2D(GL_TEXTURE_2D, l, internal_format, (w >> l) == 0 ? 1 : (w >> l), (h >> l) == 0 ? 1 : (h >> l), 0, format, GL_UNSIGNED_BYTE, nullptr);
		if (glGenerateMipmap) glGenerateMipmap(GL_TEXTURE_2D);
	}

	// set up texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, !mipmap ? filter : filter == GL_LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);

	return texture;
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor(0 / 255.0f, 0 / 255.0f, 0 / 255.0f, 1.0f);	// set clear color
	glEnable(GL_CULL_FACE);								// turn on backface culling
	glEnable(GL_DEPTH_TEST);								// turn on depth tests
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	vertex corners[4];
	corners[0].pos = vec3(-1.0f, -1.0f, 0.0f);	corners[0].tex = vec2(0.0f, 0.0f);
	corners[1].pos = vec3(+1.0f, -1.0f, 0.0f);	corners[1].tex = vec2(1.0f, 0.0f);
	corners[2].pos = vec3(+1.0f, +1.0f, 0.0f);	corners[2].tex = vec2(1.0f, 1.0f);
	corners[3].pos = vec3(-1.0f, +1.0f, 0.0f);	corners[3].tex = vec2(0.0f, 1.0f);
	vertex vertices[6] = { corners[0], corners[1], corners[2], corners[0], corners[2], corners[3] };

	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return false; }

	//텍스쳐 로딩
	TEX_SKY = create_texture(sky_image_path, true); if (!TEX_SKY) return false;

	mat4 model_matrix_map1 = mat4::translate(MAP_X / 2, -DEFAULT_HIGHT, MAP_Z / 2) ; //맵의 모델 매트릭스
	mat4 model_matrix_map2 = mat4::translate(MAP_X / 2, -DEFAULT_HIGHT, MAP_Z / 2);//맵의 모델 매트릭스
	mat4 model_matrix_map3 = mat4::translate(MAP_X / 2, -DEFAULT_HIGHT, MAP_Z / 2) * mat4::scale(scale, scale, scale); //맵의 모델 매트릭스
	mat4 model_matrix_sphere = mat4::scale(2); //sphere를 조종하는 model matrix
	mat4 model_matrix_triangle = mat4::translate(MAP_X / 2, -DEFAULT_HIGHT, MAP_Z / 2) * mat4::scale(scale, scale, scale);
	// load the mesh
	model triangle = { "../bin/mesh/Triangle.obj","triangle",model_matrix_triangle };
	models.emplace_back(triangle);
	model Map2_1 = { "../bin/mesh/Map2_1.obj" ,"Map2_1",model_matrix_map1 };
	models.emplace_back(Map2_1);
	model Map2_2 = { "../bin/mesh/Map2_2.obj" ,"Map2_2",model_matrix_map2,false };
	models.emplace_back(Map2_2);
	model Map2_3 = { "../bin/mesh/Map2_3.obj" ,"Map2_3" ,model_matrix_map3 };
	models.emplace_back(Map2_3);
	model sphere = { "../bin/mesh/MainCharacter.obj","sphere",model_matrix_sphere };
	models.emplace_back(sphere);

	if (!load_models())
		return false;

	return true;
}

void user_finalize()
{
	delete_texture_cache();
	delete pMesh;
}

int main(int argc, char* argv[])
{
	// create window and initialize OpenGL extensions
	if (!(window = cg_create_window(window_name, window_size.x, window_size.y))) { glfwTerminate(); return 1; }
	if (!cg_init_extensions(window)) { glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if (!(program = cg_create_program(vert_shader_path, frag_shader_path))) { glfwTerminate(); return 1; }	// create and compile shaders/program
	if (!user_init()) { printf("Failed to user_init()\n"); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback(window, reshape);	// callback for window resizing events
	glfwSetKeyCallback(window, keyboard);			// callback for keyboard events
	glfwSetMouseButtonCallback(window, mouse);	// callback for mouse click inputs
	glfwSetCursorPosCallback(window, motion);		// callback for mouse movement

	// enters rendering/event loop
	for (frame = 0; !glfwWindowShouldClose(window); frame++)
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