#version 410 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 color;
layout(location=2) in vec2 texture;

uniform mat4 u_modelMatrix;
uniform mat4 u_projectionMatrix;

out vec3 v_color;
out vec2 v_texCoord;

void main(){
  gl_Position = u_projectionMatrix * u_modelMatrix * vec4(position,1.0f);
  v_color = color;
  v_texCoord = texture;
}
