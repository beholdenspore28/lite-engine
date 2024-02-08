#version 460 core

in vec3 v_fragPosition;
in vec3 v_color;
in vec3 v_normal;
in vec2 v_texCoord;

out vec4 out_color;

uniform sampler2D u_texture;
uniform vec3 u_lightColor;
uniform vec3 u_lightPosition; //NEW

void main(){
	float ambientStrength = 0.1;
	vec3 ambient = u_lightColor * ambientStrength;

	vec3 normal = normalize(v_normal);
	vec3 lightDirection = normalize(u_lightPosition - v_fragPosition);

	float diffuse = max(dot(normal, lightDirection), 0.0f);

	vec4 result = vec4(ambient, 1.0) * texture(u_texture, v_texCoord) * diffuse * 2;
	// vec4 result = vec4(diffuse);
	out_color = result;
}
