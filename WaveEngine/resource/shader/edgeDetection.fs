#version 330 core

in vec2 fragUV;

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
    const int RobertsCrossX[4] = int[4](
        1, 0,
        0, -1
    );

    const int RobertsCrossY[4] = int[4](
        0, 1,
        -1, 0
    );

    vec2 tex_offset = 1.0 / textureSize(Texture, 0);
    vec3 topLeftColor = texture(Texture, fragUV).rgb;
    vec3 topRightColor = texture(Texture, fragUV + vec2(tex_offset.x, 0.0)).rgb;
    vec3 bottomLeftColor = texture(Texture, fragUV + vec2(0.0, -tex_offset.y)).rgb;
    vec3 bottomRightColor = texture(Texture, fragUV + vec2(tex_offset.x, -tex_offset.y)).rgb;

    vec3 horizontal = topLeftColor * RobertsCrossX[0]; // top left (factor +1)
    horizontal += bottomRightColor * RobertsCrossX[3]; // bottom right (factor -1)

    vec3 vertical = bottomLeftColor * RobertsCrossY[2]; // bottom left (factor -1)
    vertical += topRightColor * RobertsCrossY[1]; // top right (factor +1)

    float edge = sqrt(dot(horizontal, horizontal) + dot(vertical, vertical));

    if(edge > 0.01)
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    else
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}