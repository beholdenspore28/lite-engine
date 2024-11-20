#version 460 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D u_texture;
uniform vec3 u_color;

void main()
{
   FragColor = (texture(u_texture, texCoord) *
   texture(u_texture, texCoord));

   FragColor = vec4(0.6);
}
