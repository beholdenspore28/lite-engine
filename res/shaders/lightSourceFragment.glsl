#version 460 core

out vec4 out_color;

uniform vec3 u_lightColor;

void main(){
	out_color = vec4(u_lightColor, 1.0);
}
