#version 330 core
out vec4 FragColor;

in vec2 uv;

uniform int screenWidth;
uniform int screenHeight;

struct Camera {
    vec3 camPos;
    vec3 front;
    vec3 right;
    vec3 up;
    float halfH;
    float halfW;
    vec3 leftbottom;
    int LoopNum;
};
uniform struct Camera camera;

struct Ray{
    vec3 origin;
    vec3 direction;
    float t;
}

struct Sphere{
    vec3 origin
    vec3 radius
}

uniform float randOrigin;
uint wseed;
float randcore(uint seed) {
    seed = (seed ^ uint(61)) ^ (seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> uint(4));
    seed *= uint(0x27d4eb2d);
    wseed = seed ^ (seed >> uint(15));
    return float(wseed) * (1.0 / 4294967296.0);
}
float rand() {
    return randcore(wseed);
}

// 采样历史帧的纹理采样器
uniform sampler2D historyTexture;

void rayTracing(){
    //sphere
    Sphere sphere;
    float sRadius = sphere.radius;
    vec3 sOrigin = sphere.origin;

    // ray
    Ray ray;
    vec3 rayOrigin = ray.origin;
    vec3 rayDir = ray.direction;
    float t = ray.t;

    // solve the intersection equation
    float a = dot(rayDir, rayDir);
    float b = 2 * dot((rayOrigin - sOrigin), rayDir);
    float c = dot((rayOrigin - sOrigin), (rayOrigin - sOrigin)) - sRadius * sRadius;
    float discriminant = b * b - 4 * a * c;
    if(discriminant >= 0){
        float root = (-b - sqrt(discriminant)) / (2.0 * a);
        t = root;
        vec3 intersectionPoint = rayOrigin + t * rayDir;
        vec3 N = normalize(intersectionPoint - sOrigin);
            vec3 LightDir = normalize(vec3(1.0, 1.0, 4.0));
            float dif = max(dot(N, LightDir), 0.0);
            float spec = pow(max(dot(N, LightDir), 0.0), 64);
            float lu = 0.1 + 0.5 * dif + 0.4 * spec;
            vec3 curColor = (1.0 / float(camera.LoopNum))*vec3(rand(), rand(), rand()) + (float(camera.LoopNum -1)/float(camera.LoopNum))*hist;
            FragColor = vec4(curColor, 1.0);

    }
    else{
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}

vec3 randomUnitVec() {
    while (true) {
        vec3 unit_cube(rand(), rand(), rand());
        if (glm::dot(unit_cube, unit_cube) < 1.0f) { //为了均匀分布，否则归一化后沿立方体对角线的抽样比较多
            return glm::normalize(unit_cube);
        }
    }
    return vec3(1.0f, 0, 0);
}

void main() {
	Ray ray;
	ray.origin = camera.camPos;
	ray.direction = normalize(camera.leftbottom + (uv.x * 2.0 * camera.halfW) * camera.right + (uv.y * 2.0 * camera.halfH) * camera.up);

    wseed = uint(randOrigin * float(6.95857) * (uv.x * uv.y));
    //if (distance(uv, vec2(0.5, 0.5)) < 0.4)
    //	FragColor = vec4(rand(), rand(), rand(), 1.0);
    //else
    //	FragColor = vec4(0.0, 0.0, 0.0, 1.0);

    // 获取历史帧信息
    vec3 hist = texture(historyTexture, uv).rgb;

}





