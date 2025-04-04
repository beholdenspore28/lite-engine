#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 3) in mat4 a_instance_matrix;

uniform mat4 u_camera_matrix;
uniform mat4 u_model_matrix;
uniform bool u_use_instancing;

out vec3 v_normal;

void main() {
  mat4 mvp;
  if (u_use_instancing){
    mvp = u_camera_matrix * a_instance_matrix;
  } else {
    mvp = u_camera_matrix * u_model_matrix;
  }
	//TODO this is EXPENSIVE! do it on the cpu instead
	v_normal = mat3(transpose(inverse(mvp))) * a_normal;

  gl_Position = mvp * vec4(a_position, 1.0);
} 
