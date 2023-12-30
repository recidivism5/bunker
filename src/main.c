#include <t33.h>
#include <renderer.h>
#include <fast_obj.h>

TSTRUCT(Human){
	vec3 position;
	vec3 headEuler;
};

Human player;

TSTRUCT(WorldShader){
	char *vs, *fs;
	GLuint id;
	GLint 
		aPos,
		aNorm,
		aTexCoord,
		uMVP,
		uTex,
		uLightDir;
} worldShader = {
	"in vec3 aPos;\n"
	"in vec3 aNorm;\n"
	"in vec2 aTexCoord;\n"
	"out vec3 vNorm;\n"
	"out vec2 vTexCoord;\n"
	"uniform mat4 uMVP;\n"
	"void main(){\n"
	"	gl_Position = uMVP * vec4(aPos, 1.0);\n"
	"	vNorm = (uMVP * vec4(aNorm, 0.0)).xyz;\n"
	"	vTexCoord = aTexCoord;\n"
	"}",

	"in vec3 vNorm;\n"
	"in vec2 vTexCoord;\n"
	"uniform sampler2D uTex;\n"
	"uniform vec3 uLightDir;\n"
	"out vec4 FragColor;\n"
	"void main(){\n"
	"	FragColor = dot(uLightDir,vNorm) * texture(uTex,vTexCoord);\n"
	"}"
};

void gpu_mesh_from_obj(GPUMesh *m, char *path){
	//we want to be able to render arbitrary shit, just like in region83
	//support multiple materials, just diffuse textures
}

int main(int argc, char **argv){
	create_window("fuck",640,480);
	fastObjMesh *testWorld = fast_obj_read("../res/test_world.obj");
	if (!testWorld){
		fatal_error("Failed to read test_world.obj");
	}

	worldShader.id = compile_shader("world",worldShader.vs,worldShader.fs);
	worldShader.aPos = glGetAttribLocation(worldShader.id,"aPos");
	worldShader.aNorm = glGetAttribLocation(worldShader.id,"aNorm");
	worldShader.aTexCoord = glGetAttribLocation(worldShader.id,"aTexCoord");
	worldShader.uMVP = glGetUniformLocation(worldShader.id,"uMVP");
	worldShader.uTex = glGetUniformLocation(worldShader.id,"uTex");
	worldShader.uLightDir = glGetUniformLocation(worldShader.id,"uLightDir");

	start_main_loop();
}

void update_proc(double delta_time){
	glClearColor(1.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int width = viewport[2];
	int height = viewport[3];

	mat4 persp;
	mat4_perspective(persp,0.5f*M_PI,(float)width/(float)height,0.01f,1000.0f);
	mat4 view;
	mat4_view(view,player.headEuler,player.position);
	mat4 vp;
	mat4_mul(persp,view,vp);

	glUseProgram(worldShader.id);

}

void resize_proc(int width, int height){
	glViewport(0,0,width,height);
}

void key_proc(int key, bool down){

}

void mouse_button_proc(int button, bool down){

}

void mouse_scroll_proc(int scroll_amount){

}

void mouse_position_proc(int x, int y){

}