#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

void main(){
	gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(aPos, 1.0);
	texCoord = aTexCoord;
	normal = mat3(transpose(inverse(u_modelMatrix))) * aNormal; //TODO this is EXPENSIVE! do it on the cpu instead
	fragPos = vec3(u_modelMatrix * vec4(aPos, 1.0));
} 
