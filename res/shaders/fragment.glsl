#version 460 core

in vec2 v_texCoord;
in vec3 v_color;
out vec4 out_color;

uniform sampler2D u_texture;

void main(){
	out_color = texture(u_texture, v_texCoord) * vec4(v_color, 1.0f);
}
