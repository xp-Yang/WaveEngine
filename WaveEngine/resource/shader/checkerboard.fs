#version 330 core

in vec3 fragScaledModelPos;
in vec3 fragWorldPos;

void main()
{
    vec3 integer = floor(fragScaledModelPos);
    if((mod(integer.x, 2) == 0 && mod(integer.z, 2) == 0) ||
        (mod(integer.x, 2) == 1 && mod(integer.z, 2) == 1))
    {
        if(mod(integer.y, 2) == 0)
            gl_FragColor = vec4(0.1, 0.1, 0.1, 1.0);
        else
            gl_FragColor = vec4(0.75, 0.75, 0.75, 1.0);
    }
    else{
        if(mod(integer.y, 2) == 0)
            gl_FragColor = vec4(0.75, 0.75, 0.75, 1.0);
        else
            gl_FragColor = vec4(0.1, 0.1, 0.1, 1.0);
    }
}