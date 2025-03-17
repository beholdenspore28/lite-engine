#version 410 core

out vec4 frag_color;
in vec2 vertex_texture_coordinates;
uniform sampler2D image;

void main() {
  frag_color = texture(image, vertex_texture_coordinates);
}
