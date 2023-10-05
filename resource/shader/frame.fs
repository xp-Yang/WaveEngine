#version 330 core

in vec2 pass_uv;
uniform sampler2D Texture;
//uniform sampler2DMS Texture;
out vec4 fragment_color;
void main()
{
    fragment_color = texture(Texture, pass_uv);

    // customize anti-aliasing
    //ivec2 texSize = textureSize(Texture);
    //fragment_color = texelFetch(Texture, ivec2(pass_uv * texSize), 3);

    // debug depth
    //float depthValue = texture(Texture, pass_uv).r;
    //float z = depthValue * 2.0 - 1.0; // back to NDC 
    //float near = 0.1; 
    //float far  = 100.0; 
    //float depth = (2.0 * near * far) / (far + near - z * (far - near)) / far;
    //fragment_color = vec4(vec3(depth), 1.0);
    
    // debug color
    //fragment_color = vec4(0.2, 1.0, 0.0, 1.0);
}