#version 460 core

in vec3 v_normal;
out vec4 fragment_color;

uniform vec4 u_color;

void main() {
   fragment_color = u_color;
}
