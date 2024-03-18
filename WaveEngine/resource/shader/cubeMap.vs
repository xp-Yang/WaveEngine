#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

out vec3 fragWorldPos;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0);
    fragWorldPos = vec3(model * vec4(position, 1.0));
}