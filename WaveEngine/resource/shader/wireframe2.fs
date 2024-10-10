#version 330 core

in vec3 barycentric_coords;

out vec4 outColor;

float edgeFactor(float pixelWidth) {
    vec3 a3 = smoothstep(vec3(0.0), fwidth(barycentric_coords) * pixelWidth, barycentric_coords);
    return min(min(a3.x, a3.y), a3.z);
}

void main() {
    vec4 wireframeColor = vec4(0.18, 0.18, 0.18, 1.0);
    outColor = mix(wireframeColor, vec4(0.0), edgeFactor(1.0));
}