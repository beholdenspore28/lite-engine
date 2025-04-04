#version 460 core

in vec3 v_normal;
out vec4 fragment_color;

uniform vec4 u_color;

void main() {
  vec3 light_direction = normalize(vec3(1,1,0));
  float diffuse = max(dot(v_normal, light_direction), 0.0) * 0.1;
   fragment_color = u_color + diffuse;
}
