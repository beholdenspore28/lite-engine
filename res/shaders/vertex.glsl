#version 410 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 color;

uniform mat4 u_modelMatrix;

out vec3 v_color;

void main(){
  v_color = color;
  gl_Position = u_modelMatrix * vec4(position,1.0f);
}
