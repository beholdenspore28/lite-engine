#version 460 core

in vec2 v_tex_coord;
out vec4 FragColor;

uniform sampler2D u_texture;

void main() {
   FragColor = texture(u_texture, v_tex_coord);
}
