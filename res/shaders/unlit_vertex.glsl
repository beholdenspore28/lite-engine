#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

void main(){
	gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(aPos, 1.0);
	texCoord = aTexCoord;
} 
