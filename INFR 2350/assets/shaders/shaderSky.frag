#version 420

layout(std140, binding = 1) uniform Time
{
	uniform float uTime;
};

layout(binding = 0) uniform samplerCube uTexCube;

in vec2 texcoord;
in vec3 norm;
in vec3 pos;

out vec4 outColor;

void main()
{
	outColor = texture(uTexCube, normalize(norm));
}
