#include <renderer.h>
#include <qoi.h>

GLenum glCheckError_(const char *file, int line){
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR){
		char *error;
		switch (errorCode){
		case GL_INVALID_ENUM:      error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:     error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:     error = "OUT_OF_MEMORY"; break;
		default: error = "UNKNOWN TYPE BEAT";break;
		}
		fatal_error("%s %s (%d)",error,file,line);
	}
	return errorCode;
}

void texture_from_file(Texture *t, char *path){
	qoi_desc desc;
	void *pixels = qoi_read(path,&desc,4);
	t->width = desc.width;
	t->height = desc.height;
	glGenTextures(1,&t->id);
	glBindTexture(GL_TEXTURE_2D,t->id);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,t->width,t->height,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
	free(pixels);
}

void check_shader(char *name, char *type, GLuint id){
	GLint result;
	glGetShaderiv(id,GL_COMPILE_STATUS,&result);
	if (!result){
		char infolog[512];
		glGetShaderInfoLog(id,512,NULL,infolog);
		fatal_error("%s %s shader compile error: %s",name,type,infolog);
	}
}

void check_program(char *name, char *status_name, GLuint id, GLenum param){
	GLint result;
	glGetProgramiv(id,param,&result);
	if (!result){
		char infolog[512];
		glGetProgramInfoLog(id,512,NULL,infolog);
		fatal_error("%s shader %s error: %s",name,status_name,infolog);
	}
}

GLuint compile_shader(char *name, char *vert_src, char *frag_src){
	GLuint v = glCreateShader(GL_VERTEX_SHADER);
	GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(v,1,&vert_src,NULL);
	glShaderSource(f,1,&frag_src,NULL);
	glCompileShader(v);
	check_shader(name,"vertex",v);
	glCompileShader(f);
	check_shader(name,"fragment",f);
	GLuint p = glCreateProgram();
	glAttachShader(p,v);
	glAttachShader(p,f);
	glLinkProgram(p);
	check_program(name,"link",p,GL_LINK_STATUS);
	glDeleteShader(v);
	glDeleteShader(f);
	return p;
}

TextureLinkedHashListBucket *TextureLinkedHashListGet(TextureLinkedHashList *list, char *key, int keylen){
	if (!list->total) return 0;
	int index = fnv_1a(key,keylen) % list->total;
	TextureLinkedHashListBucket *tombstone = 0;
	while (1){
		TextureLinkedHashListBucket *b = list->buckets+index;
		if (b->key == UINTPTR_MAX) tombstone = b;
		else if (b->key == 0) return tombstone ? tombstone : b;
		else if (!memcmp(b->key,key,keylen)) return b;
		index = (index + 1) % list->total;
	}
}

TextureLinkedHashListBucket *TextureLinkedHashListGetChecked(TextureLinkedHashList *list, char *key, int keylen){
	TextureLinkedHashListBucket *b = TextureLinkedHashListGet(list,key,keylen);
	if (!b || b->key == 0 || b->key == UINTPTR_MAX) return 0;
	return b;
}

intLinkedHashListBucket *intLinkedHashListGet(intLinkedHashList *list, char *key, int keylen){
	if (!list->total) return 0;
	int index = fnv_1a(key,keylen) % list->total;
	intLinkedHashListBucket *tombstone = 0;
	while (1){
		intLinkedHashListBucket *b = list->buckets+index;
		if (b->key == TOMBSTONE) tombstone = b;
		else if (b->key == 0) return tombstone ? tombstone : b;
		else if (!memcmp(b->key,key,keylen)) return b;
		index = (index + 1) % list->total;
	}
}

intLinkedHashListBucket *intLinkedHashListGetChecked(intLinkedHashList *list, char *key, int keylen){
	intLinkedHashListBucket *b = intLinkedHashListGet(list,key,keylen);
	if (!b || b->key == 0 || b->key == TOMBSTONE) return 0;
	return b;
}

void intLinkedHashListRemove(intLinkedHashList *list, intLinkedHashListBucket *b){
	b->key = TOMBSTONE;
	if (b->prev) b->prev->next = b->next;
	if (b->next) b->next->prev = b->prev;
	if (list->first == b) list->first = b->next;
	if (list->last == b) list->last = b->prev;
	list->used--;
	list->tombstones++;
}

intLinkedHashListBucket *intLinkedHashListNew(intLinkedHashList *list, char *key, int keylen){
	if ((list->used+list->tombstones+1) > (list->total*3)/4){ // 3/4 load limit
		intLinkedHashList newList;
		newList.total = 16; // same as used resize factor
		while (newList.total < (list->used*16)) newList.total *= 2; // 16x used resize
		newList.used = list->used;
		newList.tombstones = 0;
		newList.first = 0;
		newList.last = 0;
		newList.buckets = zalloc_or_die(newList.total*sizeof(*newList.buckets));
		for (intLinkedHashListBucket *b = list->first; b; b = b->next){
			intLinkedHashListBucket *nb = intLinkedHashListGet(&newList,b->key,b->keylen);
			nb->key = b->key;
			nb->keylen = b->keylen;
			nb->value = b->value;
			if (!newList.first){
				newList.first = nb;
				nb->prev = 0;
			} else {
				newList.last->next = nb;
				nb->prev = newList.last;
			}
			newList.last = nb;
			nb->next = 0;
		}
		if (list->buckets) free(list->buckets);
		*list = newList;
	}
	intLinkedHashListBucket *b = intLinkedHashListGet(list,key,keylen);
	b->key = key;
	b->keylen = keylen;
	if (!list->first){
		list->first = b;
		b->prev = 0;
	} else {
		list->last->next = b;
		b->prev = list->last;
	}
	list->last = b;
	b->next = 0;
	if (b->key == TOMBSTONE) list->tombstones--;
	list->used++;
	return b;
}