#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 coordinate;
layout(location = 3) in vec3 normal;

layout(location = 0) out vec3 posn;
layout(location = 2) out vec2 coord;
layout(location = 3) out vec3 norm;

void main() {
	posn = position;
	coord = coordinate;
	norm = normal;
	gl_Position = vec4(posn, 1.0);
}