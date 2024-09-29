#version 330 core

in vec3 fragScaledModelPos;
in vec3 fragWorldPos;

out vec4 outColor;

void main()
{
    vec3 i = fragScaledModelPos / 2.0;
    vec3 f = fract(i);
    bool black = ((f.x < 0.5 && f.z < 0.5) || (f.x > 0.5 && f.z > 0.5)) && (f.y < 0.5);
    black = black || (((f.x < 0.5 && f.z > 0.5) || (f.x > 0.5 && f.z < 0.5)) && (f.y > 0.5));
    outColor = vec4(vec3(0.75) - float(black) * vec3(0.65), 1.0);
}