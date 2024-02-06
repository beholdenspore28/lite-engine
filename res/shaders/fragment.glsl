#version 460 core

in vec2 v_texCoord;
in vec3 v_color;
out vec4 out_color;

uniform sampler2D u_texture;
uniform vec3 objectColor; //TODO: figure out how to include vertex colors into the object color
uniform vec3 lightColor;

void main(){
	out_color = texture(u_texture, v_texCoord) * vec4(lightColor, 1.0f);
}
