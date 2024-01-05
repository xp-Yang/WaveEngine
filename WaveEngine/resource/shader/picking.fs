#version 330 core

uniform vec4 picking_color;

void main(){
    gl_FragColor = picking_color;
}