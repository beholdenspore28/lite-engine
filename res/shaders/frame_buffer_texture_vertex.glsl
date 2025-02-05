#version 410 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texture_coordinates;

out vec2 vertex_texture_coordinates;

void main() {
  gl_Position = vec4(in_position.x, in_position.y, 0.0, 1.0); 
  vertex_texture_coordinates = in_texture_coordinates;
}  
