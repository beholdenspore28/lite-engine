#version 410 core

int LIGHT_POINT = 0;
int LIGHT_SPOT = 1;
int LIGHT_DIRECTIONAL = 2;

struct light_t {
	int type;
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;

uniform vec3 u_cameraPos;
uniform Material u_material;
uniform vec3 u_ambientLight;
uniform light_t u_light;

vec3 lightDirectional(light_t light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),u_material.shininess);
    // combine results
    vec3 ambient = u_ambientLight * vec3(texture(u_material.diffuse, texCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(u_material.diffuse, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(u_material.specular, texCoord));
    return (ambient + diffuse + specular);
}

vec3 lightPoint(light_t light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = u_ambientLight * vec3(texture(u_material.diffuse, texCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(u_material.diffuse, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(u_material.specular, texCoord));
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
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);
    // combine results
    vec3 ambient = u_ambientLight * vec3(texture(u_material.diffuse, texCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(u_material.diffuse, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(u_material.specular, texCoord));
    return (ambient + diffuse + specular);
}

vec3 lightSpot(light_t light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    // intensity
    float theta = dot(lightDir, normalize(light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambient = u_ambientLight * vec3(texture(u_material.diffuse, texCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(u_material.diffuse, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(u_material.specular, texCoord));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

void main() {
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(u_cameraPos - fragPos);

	vec3 light = lightPointInfiniteRange(u_light, norm, fragPos, viewDir);
	
	fragColor = vec4(light, 1.0);
//	fragColor = vec4(1.0);
}
