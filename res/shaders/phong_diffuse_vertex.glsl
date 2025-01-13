#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

uniform mat4 u_mvp;

void main(){
	gl_Position = u_mvp * vec4(aPos, 1.0);
	texCoord = aTexCoord;
	normal = mat3(transpose(inverse(u_mvp))) * aNormal; //TODO this is EXPENSIVE! do it on the cpu instead
	fragPos = vec3(u_mvp * vec4(aPos, 1.0));
} 
