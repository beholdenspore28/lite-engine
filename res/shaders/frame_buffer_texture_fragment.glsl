#version 410 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D u_diffuse_map;

const float offset = 1.0 / 300.0;

void main() { 
    //{ // default
    //  FragColor = texture(u_diffuse_map, TexCoords);
    //}

    //{ // invert color
    //  FragColor = vec4(vec3(1.0 - texture(u_diffuse_map, TexCoords)), 1.0);
    //}

    //{ // grayscale
    //  FragColor = texture(u_diffuse_map, TexCoords);
    //  float average =
    //    0.2126 * FragColor.r +
    //    0.7152 * FragColor.g +
    //    0.0722 * FragColor.b;
    //  FragColor = vec4(average, average, average, 1.0);
    //}

    vec2 offsets[9] = vec2[](
      vec2(-offset,  offset), // top left
      vec2( 0.0,     offset), // top center
      vec2( offset,  offset), // top right

      vec2(-offset,  0.0),    // center left
      vec2( 0.0,     0.0),    // center center
      vec2( offset,  0.0),    // center right

      vec2(-offset, -offset), // bottom left
      vec2( 0.0,    -offset), // bottom center
      vec2( offset, -offset) // bottom right
    );

    //float kernel[9] = float[](
    //  -1,     -1,     -1,   
    //  -1,      9,     -1,
    //  -1,     -1,     -1
    //);
    //float kernel[9] = float[](
    //  1.0 / 16,     2.0 / 16,    1.0 / 16,   
    //  2.0 / 16,     4.0 / 16,    2.0 / 16,
    //  1.0 / 16,     2.0 / 16,    1.0 / 16
    //);

    float kernel[9] = float[](
      1.0,  1.0,  1.0,
      1.0, -8.0,  1.0,
      1.0,  1.0,  1.0
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++) {
      sampleTex[i] = vec3(texture(u_diffuse_map, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0);
    for(int i = 0; i < 9; i++)
      col += sampleTex[i] * kernel[i];

    FragColor = vec4(col, 1.0);
}
