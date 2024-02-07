#version 460 core

layout(location=0) in vec3 in_position; 	//initial vertex position
layout(location=1) in vec2 in_texCoord; 	//initial vertex texture cordinate
layout(location=2) in vec3 in_normal;
// layout(location=3) in vec3 in_color; 		//initial vertex color

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

// out vec3 v_color;
out vec2 v_texCoord;

void main(){
  gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * 
			vec4(in_position,1.0f);

  // v_color = in_color;
  v_texCoord = in_texCoord;
}
