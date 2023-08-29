#version 330 core

uniform vec4 picking_color;

out vec4 out_color;

void main(){
    out_color = picking_color;
}