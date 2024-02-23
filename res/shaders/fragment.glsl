#version 460 core

in vec3 v_fragPosition;
in vec3 v_color;
in vec3 v_normal;
in vec2 v_texCoord;

out vec4 out_color;

uniform sampler2D u_texture;
uniform vec3 u_lightColor;
uniform vec3 u_lightPosition; //NEW
uniform vec3 u_cameraPosition;

void main(){
	
	//ambient light
	float ambient = 0.1;

	//diffuse light
	vec3 normal = normalize(v_normal);
	vec3 lightDirection = normalize(u_lightPosition - v_fragPosition);
	float diffuseStrength = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = u_lightColor * diffuseStrength;

	//specular light
	float specularStrength = 0.5;
	vec3 viewDirection = normalize(-u_cameraPosition - v_fragPosition);
	vec3 reflectDirection = reflect(-lightDirection, normal);
	float specularValue = pow(max(dot(viewDirection, reflectDirection), 0.0),32);
	vec3 specular = specularStrength * specularValue * u_lightColor;
	
	//final light color
	out_color = texture(u_texture, v_texCoord) * vec4(ambient + diffuse + specular, 1.0);
}
