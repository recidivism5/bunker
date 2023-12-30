#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#define LERP(a,b,t) ((a) + (t)*((b)-(a)))

typedef float vec3[3];
typedef float vec4[4];
typedef int ivec3[3];
typedef vec4 mat4[4];

inline void vec3_copy(vec3 dst, vec3 src){
	memcpy(dst,src,sizeof(dst));
}
inline void vec4_copy(vec4 dst, vec4 src){
	memcpy(dst,src,sizeof(dst));
}
inline void mat4_copy(mat4 dst, mat4 src){
	memcpy(dst,src,sizeof(dst));
}
inline float vec3_dot(vec3 a, vec3 b){
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}
inline float vec3_length(vec3 a){
	return sqrtf(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
}
inline float vec3_angle_between(vec3 a, vec3 b){
	return acosf((a[0]*b[0] + a[1]*b[1] + a[2]*b[2])/(sqrtf(a[0]*a[0] + a[1]*a[1] + a[2]*a[2])*sqrtf(b[0]*b[0] + b[1]*b[1] + b[2]*b[2])));
}
inline void vec3_cross(vec3 a, vec3 b, vec3 out){
	out[0] = a[1]*b[2] - a[2]*b[1];
	out[1] = a[2]*b[0] - a[0]*b[2];
	out[2] = a[0]*b[1] - a[1]*b[0];
}
inline void vec3_add(vec3 a, vec3 b){
	for (int i = 0; i < 3; i++) a[i] += b[i];
}
inline void vec3_subtract(vec3 a, vec3 b){
	for (int i = 0; i < 3; i++) a[i] -= b[i];
}
inline void vec3_negate(vec3 a){
	for (int i = 0; i < 3; i++) a[i] = -a[i];
}
inline void vec3_scale(vec3 a, float scale){
	for (int i = 0; i < 3; i++) a[i] = a[i]*scale;
}
inline void vec3_normalize(vec3 a){
	float d = 1.0f / sqrtf(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
	for (int i = 0; i < 3; i++) a[i] *= d;
}
inline void vec3_set_length(vec3 a, float length){
	float d = length / sqrtf(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
	for (int i = 0; i < 3; i++) a[i] *= d;
}
inline void vec3_midpoint(vec3 a, vec3 b, vec3 out){
	for (int i = 0; i < 3; i++) out[i] = a[i] + 0.5f*(b[i]-a[i]);
}
inline void vec3_lerp(vec3 a, vec3 b, float t){
	for (int i = 0; i < 3; i++) a[i] = LERP(a[i],b[i],t);
}
inline void vec3_clamp_euler(vec3 a){
	for (int i = 0; i < 3; i++){
		if (a[i] > 4.0f*M_PI) a[i] -= 4.0f*M_PI;
		else if (a[i] < -4.0f*M_PI) a[i] += 4.0f*M_PI;
	}
}
inline void quat_slerp(vec4 a, vec4 b, float t){//from https://github.com/microsoft/referencesource/blob/51cf7850defa8a17d815b4700b67116e3fa283c2/System.Numerics/System/Numerics/Quaternion.cs#L289C8-L289C8
	float cosOmega = a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3];
	int flip = 0;
	if (cosOmega < 0.0f){
		flip = 1;
		cosOmega = -cosOmega;
	}
	float s1,s2;
	if (cosOmega > (1.0f - 1e-6f)){
		s1 = 1.0f - t;
		s2 = flip ? -t : t;
	} else {
		float omega = acosf(cosOmega);
		float invSinOmega = 1.0f / sinf(omega);
		s1 = sinf((1.0f-t)*omega)*invSinOmega;
		float sm = sinf(t*omega)*invSinOmega;
		s2 = flip ? -sm : sm;
	}
	for (int i = 0; i < 4; i++) a[i] = s1*a[i] + s2*b[i];
}
inline void mat4_transpose(mat4 m){
	float t;
	SWAP(t,m[0][1],m[1][0]);
	SWAP(t,m[0][2],m[2][0]);
	SWAP(t,m[0][3],m[3][0]);
	SWAP(t,m[1][2],m[2][1]);
	SWAP(t,m[1][3],m[3][1]);
	SWAP(t,m[2][3],m[3][2]);
}
inline void mat4_orthogonal(mat4 m, float left, float right, float bottom, float top, float near, float far){
	m[0][0] = 2.0f/(right-left);
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = 2.0f/(top-bottom);
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;

	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 2.0f/(near-far);
	m[2][3] = 0.0f;

	m[3][0] = (right+left)/(left-right);
	m[3][1] = (top+bottom)/(bottom-top);
	m[3][2] = (far+near)/(near-far);
	m[3][3] = 1.0f;
}
inline void mat4_perspective(mat4 m, float fovRadians, float aspectRatio, float near, float far){
	float s = 1.0f / tanf(fovRadians * 0.5f);
	float d = near - far;

	m[0][0] = s/aspectRatio;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = s;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;

	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = (far+near)/d;
	m[2][3] = -1.0f;

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = (2.0f*far*near)/d;
	m[3][3] = 0.0f;
}
inline void mat4_translation(mat4 m, vec3 translation){
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;

	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = 0.0f;

	m[3][0] = translation[0];
	m[3][1] = translation[1];
	m[3][2] = translation[2];
	m[3][3] = 1.0f;
}
inline void mat4_euler_zyx(mat4 dest, vec3 angles){//from cglm, thanks bro
	float cx, cy, cz,
		sx, sy, sz, czsx, cxcz, sysz;

	sx   = sinf(angles[0]); cx = cosf(angles[0]);
	sy   = sinf(angles[1]); cy = cosf(angles[1]);
	sz   = sinf(angles[2]); cz = cosf(angles[2]);

	czsx = cz * sx;
	cxcz = cx * cz;
	sysz = sy * sz;

	dest[0][0] =  cy * cz;
	dest[0][1] =  cy * sz;
	dest[0][2] = -sy;
	dest[1][0] =  czsx * sy - cx * sz;
	dest[1][1] =  cxcz + sx * sysz;
	dest[1][2] =  cy * sx;
	dest[2][0] =  cxcz * sy + sx * sz;
	dest[2][1] = -czsx + cx * sysz;
	dest[2][2] =  cx * cy;
	dest[0][3] =  0.0f;
	dest[1][3] =  0.0f;
	dest[2][3] =  0.0f;
	dest[3][0] =  0.0f;
	dest[3][1] =  0.0f;
	dest[3][2] =  0.0f;
	dest[3][3] =  1.0f;
}
inline void mat4_mul(mat4 a, mat4 b, mat4 out){
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			out[i][j] = a[0][j]*b[i][0] + a[1][j]*b[i][1] + a[2][j]*b[i][2] + a[3][j]*b[i][3];
}
inline void mat4_mul_vec4(mat4 m, vec4 v, vec4 out){
	for (int i = 0; i < 4; i++) out[i] = v[0]*m[0][i] + v[1]*m[1][i] + v[2]*m[2][i] + v[3]*m[3][i];
}
inline void mat4_view(mat4 m, vec3 euler, vec3 translation){
	mat4 rot;
	mat4_euler_zyx(rot,euler);
	mat4_transpose(rot);
	mat4 trans;
	vec3 ntrans;
	vec3_copy(ntrans,translation);
	vec3_negate(ntrans);
	mat4_translation(trans,ntrans);
	mat4_mul(rot,trans,m);
}