#version 460 core

out vec4 fragment_color;

uniform vec4 u_color;

void main() {
   fragment_color = u_color;
}
