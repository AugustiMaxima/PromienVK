#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 posn;
layout(location = 2) in vec2 coord;
layout(location = 3) in vec3 norm;

layout(location = 0) out vec4 color;

void main() {
    vec3 lightSource = vec3(0, -10, 10);
    vec3 gaze = vec3(0,0,1);
    vec3 incoming = lightSource - posn;
    vec3 reflection = reflect(incoming, norm);
    
    float on = dot(reflection, gaze);

    color = vec4(0.1,0.1,0.1,1);
    if(on<0)
        color += vec4(0.01,0.01,0.01,0) + -1 * on * vec4(0.5,0.5,0.5,0);
}