#version 410 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;
layout (location = 3) in mat4 a_instance_matrix;

out vec2 v_tex_coord;
out vec3 v_normal;
out vec3 v_fragment_position;

uniform vec2 u_texture_offset;
uniform vec2 u_texture_scale;
uniform vec4 u_color;

uniform mat4 u_camera_matrix;
uniform mat4 u_model_matrix;
uniform bool u_use_instancing;

void main(){
  mat4 mvp;
  if(u_use_instancing){
    mvp = u_camera_matrix * a_instance_matrix;
  }else{
    mvp = u_camera_matrix * u_model_matrix;
  }

	v_fragment_position = vec3(mvp * vec4(a_position, 1.0));
	v_tex_coord = (a_tex_coord * u_texture_scale) + u_texture_offset;

	//TODO this is EXPENSIVE! do it on the cpu instead
	v_normal = mat3(transpose(inverse(mvp))) * a_normal;

	gl_Position = mvp * vec4(a_position, 1.0);
} 
