#version 330 core

in VS_OUT {
    vec3 fragWorldPos;
    vec3 fragWorldNormal;
    vec2 fragUV;
} fs_in;

void main()
{             
    gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
    //gl_FragColor = vec4(0.2, 1.0, 0.0, 1.0);
}