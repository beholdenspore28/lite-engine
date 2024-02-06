#version 460 core

in vec2 v_texCoord;
in vec3 v_color;
out vec4 out_color;

uniform sampler2D u_texture;
uniform vec3 u_lightColor;

void main(){
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * u_lightColor;
	vec4 result = vec4(ambient, 1.0) * texture(u_texture, v_texCoord);
	out_color = result;
}
