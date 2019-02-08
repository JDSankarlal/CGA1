#version 420

layout(std140, binding = 0) uniform Camera
{
	uniform mat4 uProj;
	uniform mat4 uView;
};
uniform mat4 uModel;

layout(location = 0) in vec3 in_vert;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;

out vec2 texcoord;
out vec3 norm;
out vec3 pos;

void main()
{
	texcoord = in_uv;
	norm = mat3(uView) * mat3(uModel) * in_normal;

	pos = (uView * uModel * vec4(in_vert, 1.0f)).xyz;

	gl_Position = uProj * vec4(pos, 1.0f);
}
