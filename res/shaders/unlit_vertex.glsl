#version 410 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_tex_coord;

out vec2 v_tex_coord;

uniform mat4 u_mvp;

void main(){
	gl_Position = u_mvp * vec4(in_position, 1.0);
	v_tex_coord = in_tex_coord;
} 
