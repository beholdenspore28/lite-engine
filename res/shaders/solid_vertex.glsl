#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 3) in mat4 a_instance_matrix;

uniform mat4 u_camera_matrix;
uniform mat4 u_model_matrix;
uniform bool u_use_instancing;

void main() {
  if (u_use_instancing){
    gl_Position =
      u_camera_matrix * u_model_matrix * a_instance_matrix * vec4(a_position, 1.0);
  } else {
    gl_Position =
      u_camera_matrix * u_model_matrix * vec4(a_position, 1.0);
  }
} 
