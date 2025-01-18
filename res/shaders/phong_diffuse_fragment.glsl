#version 410 core

struct light_t {
	int		type;
	vec3		position;
	vec3		direction;
	float		cut_off;
	float		outer_cut_off;
	float		constant;
	float		linear;
	float		quadratic;
	vec3		diffuse;
	vec3		specular;
};

struct Material {
	sampler2D	diffuse;
	sampler2D	specular;
	float	shininess;
}; 

in vec3		v_fragment_position;
in vec3		v_normal;
in vec2		v_tex_coord;

out vec4	fragColor;

uniform		vec3 u_cameraPos;
uniform		Material u_material;
uniform		vec3 u_ambientLight;
uniform		light_t u_light;

vec3 lightDirectional(light_t light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(-light.direction);

	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfWay = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfWay), 0.0),u_material.shininess);

	// combine results
	vec3 ambient = u_ambientLight * vec3(texture(u_material.diffuse, v_tex_coord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(u_material.diffuse, v_tex_coord));
	vec3 specular = light.specular * spec * vec3(texture(u_material.specular, v_tex_coord));
	return (ambient + diffuse + specular);
}

vec3 lightPoint(light_t light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);

	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfWay = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfWay), 0.0), u_material.shininess);

	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// combine results
	vec3 ambient = u_ambientLight * vec3(texture(u_material.diffuse, v_tex_coord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(u_material.diffuse, v_tex_coord));
	vec3 specular = light.specular * spec * vec3(texture(u_material.specular, v_tex_coord));
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

vec3 lightPointInfiniteRange(light_t light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);

	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfWay = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfWay), 0.0), u_material.shininess);

	// combine results
	vec3 ambient = u_ambientLight * vec3(texture(u_material.diffuse, v_tex_coord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(u_material.diffuse, v_tex_coord));
	vec3 specular = light.specular * spec * vec3(texture(u_material.specular, v_tex_coord));
	return (ambient + diffuse + specular);
}

vec3 lightSpot(light_t light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);

	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfWay = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfWay), 0.0), u_material.shininess);

	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// intensity
	float theta = dot(lightDir, normalize(light.direction)); 
	float epsilon = light.cut_off - light.outer_cut_off;
	float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

	// combine results
	vec3 ambient = u_ambientLight * vec3(texture(u_material.diffuse, v_tex_coord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(u_material.diffuse, v_tex_coord));
	vec3 specular = light.specular * spec * vec3(texture(u_material.specular, v_tex_coord));
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	return (ambient + diffuse + specular);
}

void main() {
	vec3 norm = normalize(v_normal);
	vec3 viewDir = normalize(u_cameraPos - v_fragment_position);

	vec3 light = lightPoint(u_light, norm, v_fragment_position, viewDir);

	fragColor = vec4(light, 1.0);
}
