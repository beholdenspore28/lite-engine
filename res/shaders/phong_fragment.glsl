#version 410 core

struct light_t {
  int        type;
  vec3       position;
  vec3       direction;
  float      cut_off;
  float      outer_cut_off;
  float      constant;
  float      linear;
  float      quadratic;
  vec3       diffuse;
  vec3       specular;
};

struct Material {
  sampler2D  diffuse;
  sampler2D  specular;
  float      shininess;
}; 

in vec3      v_fragment_position;
in vec3      v_normal;
in vec2      v_tex_coord;

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bloom_color;

uniform      vec3     u_cameraPos;
uniform      Material u_material;
uniform      vec3     u_ambient_light;

#define      LIGHTS_MAX 32

uniform      uint    u_lights_count;
uniform      light_t u_lights[LIGHTS_MAX];

vec3 light_directional(light_t light, vec3 normal, vec3 view_direction) {
  vec3 lightDir = normalize(-light.direction);

  // diffuse shading
  float diffuse_scale = max(dot(normal, lightDir), 0.0);

  // specular shading
  vec3 half_way = normalize(lightDir + view_direction);
  float specular_scale =
    pow(max(dot(normal, half_way), 0.0),u_material.shininess);

  // combine results
  vec3 ambient = u_ambient_light * vec3(
      texture(u_material.diffuse, v_tex_coord));

  vec3 diffuse = light.diffuse * diffuse_scale * vec3(
      texture(u_material.diffuse, v_tex_coord));

  vec3 specular = light.specular * specular_scale * vec3(
      texture(u_material.specular, v_tex_coord));

  return (ambient + diffuse + specular);
}

vec3 light_point(
    light_t light,
    vec3 normal,
    vec3 fragment_position,
    vec3 view_direction) {

  vec3 lightDir = normalize(light.position - fragment_position);

  // diffuse shading
  float diffuse_scale = max(dot(normal, lightDir), 0.0);

  // specular shading
  vec3 half_way = normalize(lightDir + view_direction);
  float specular_scale =
    pow(max(dot(normal, half_way), 0.0), u_material.shininess);

  // attenuation
  float distance = length(light.position - fragment_position);

  float attenuation = 1.0 / (light.constant + light.linear * distance +
      light.quadratic * (distance * distance));

  // combine results
  vec3 ambient = u_ambient_light * vec3(
      texture(u_material.diffuse, v_tex_coord));

  vec3 diffuse = light.diffuse * diffuse_scale * vec3(
      texture(u_material.diffuse, v_tex_coord));

  vec3 specular = light.specular * specular_scale * vec3(
      texture(u_material.specular, v_tex_coord));

  diffuse *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
}

vec3 light_point_infinite_range(
    light_t light,
    vec3 normal,
    vec3 fragment_position,
    vec3 view_direction) {

  vec3 lightDir = normalize(light.position - fragment_position);

  // diffuse shading
  float diffuse_scale = max(dot(normal, lightDir), 0.0);

  // specular shading
  vec3 half_way = normalize(lightDir + view_direction);
  float specular_scale =
    pow(max(dot(normal, half_way), 0.0), u_material.shininess);

  // combine results
  vec3 ambient = u_ambient_light * vec3(
      texture(u_material.diffuse, v_tex_coord));

  vec3 diffuse = light.diffuse * diffuse_scale * vec3(
      texture(u_material.diffuse, v_tex_coord));

  vec3 specular = light.specular * specular_scale * vec3(
      texture(u_material.specular, v_tex_coord));

  return (ambient + diffuse + specular);
}

vec3 light_spot(
    light_t light,
    vec3 normal,
    vec3 fragment_position,
    vec3 view_direction) {
  vec3 lightDir = normalize(light.position - fragment_position);

  // diffuse shading
  float diffuse_scale = max(dot(normal, lightDir), 0.0);

  // specular shading
  vec3 half_way = normalize(lightDir + view_direction);

  float specular_scale =
    pow(max(dot(normal, half_way), 0.0), u_material.shininess);

  // attenuation
  float distance = length(light.position - fragment_position);

  float attenuation = 1.0 / (light.constant + light.linear * distance +
      light.quadratic * (distance * distance));

  // intensity
  float theta = dot(lightDir, normalize(light.direction)); 
  float epsilon = light.cut_off - light.outer_cut_off;
  float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

  // combine results
  vec3 ambient = u_ambient_light * vec3(
      texture(u_material.diffuse, v_tex_coord));

  vec3 diffuse = light.diffuse * diffuse_scale * vec3(
      texture(u_material.diffuse, v_tex_coord));

  vec3 specular = light.specular * specular_scale * vec3(
      texture(u_material.specular, v_tex_coord));

  ambient *= attenuation * intensity;
  diffuse *= attenuation * intensity;
  specular *= attenuation * intensity;
  return (ambient + diffuse + specular);
}

void main() {
  vec3 norm = normalize(v_normal);
  vec3 view_direction = normalize(u_cameraPos - v_fragment_position);

  vec3 light = vec3(0,0,0);
  for(int i = 0; i < u_lights_count; i++) {
    if (i >= LIGHTS_MAX) { break; };
    light += light_point(
        u_lights[i], norm, v_fragment_position, view_direction);
  }

  frag_color = vec4(light, 1.0);
  float brightness = dot(frag_color.rgb, vec3(0.2126, 0.7152, 0.0722));
  
  if(brightness > 1.0) {
    bloom_color = vec4(frag_color.rgb, 1.0);
  } else {
    bloom_color = vec4(0.0, 0.0, 0.0, 1.0);
  }
}
