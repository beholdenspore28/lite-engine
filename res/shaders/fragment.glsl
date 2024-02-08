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
	float ambient = 0.1;

	vec3 normal = normalize(v_normal);
	vec3 lightDirection = normalize(u_lightPosition - v_fragPosition);

	float diffuseStrength = max(dot(normal, lightDirection), 0.0f);
	vec3 diffuse = u_lightColor * diffuseStrength;

	vec4 result = texture(u_texture, v_texCoord) * vec4(ambient + diffuse, 1.0);
	out_color = result;
}
