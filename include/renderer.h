#pragma once

#include <t33.h>
#include <fast_obj.h>

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

TSTRUCT(Texture){
	GLuint id;
	int width, height;
};

void texture_from_file(Texture *t, char *path);

void check_shader(char *name, char *type, GLuint id);

void check_program(char *name, char *status_name, GLuint id, GLenum param);

GLuint compile_shader(char *name, char *vert_src, char *frag_src);

//we need a texture hashlist
TSTRUCT(TextureLinkedHashListBucket){
	TextureLinkedHashListBucket *prev, *next;
	char *key;
	int keylen;
	Texture value;
};

TSTRUCT(TextureLinkedHashList){
	int total, used, tombstones;
	TextureLinkedHashListBucket *buckets, *first, *last;
};

TSTRUCT(GPUMesh){
	fastObjMesh *obj;
	GLuint vao, vbo;
	int *indices;
	int indicesCount;
};