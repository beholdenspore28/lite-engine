#version 410 core

in vec2 v_texCoord;
// in vec3 v_color;
out vec4 f_color;

uniform sampler2D texture;

void main(){
	f_color = texture2D(texture, v_texCoord);
}
