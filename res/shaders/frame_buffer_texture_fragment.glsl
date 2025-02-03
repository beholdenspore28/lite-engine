#version 410 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D u_diffuse_map;

void main()
{ 
    FragColor = texture(u_diffuse_map, TexCoords);
}
