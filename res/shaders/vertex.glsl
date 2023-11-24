#version 460 core

layout(location=0) in vec3 i_position;
layout(location=1) in vec3 i_color;
layout(location=2) in vec2 i_texCoord;

uniform mat4 u_modelMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_viewMatrix;

out vec3 v_color;
out vec2 v_texCoord;

void main(){
  gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * 
			vec4(i_position,1.0f);

  v_color = i_color;
  v_texCoord = i_texCoord;
}
