#version 330 core

in vec3 fragWorldPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    float lightDistance = length(fragWorldPos - lightPos);

    lightDistance = lightDistance / far_plane;

    gl_FragDepth = lightDistance;
}