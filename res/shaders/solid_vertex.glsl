#version 410 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_tex_coord;

out vec4 v_color;

uniform vec4 u_color;
uniform mat4 u_mvp;

void main(){
	gl_Position = u_mvp * vec4(in_position, 1.0);
  v_color = u_color;
} 
