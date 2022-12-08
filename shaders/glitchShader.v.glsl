#version 410 core

// Uniforms
uniform mat4 mvpMtx;

// Attrs
in vec3 vPos;

//vary
out vec4 fPos;

float noise(float pos) {
    float noiseVal = pos * 100;
    noiseVal = noiseVal - floor(noiseVal);
    return noiseVal;
}
void main() {
    vec4 transformedPoint = mvpMtx * vec4(vPos, 1.0);
    vec3 noiseVec = vec3(noise(transformedPoint.x), noise(transformedPoint.y), noise(transformedPoint.z));
    float noiseScale = 1.0;

    vec4 outputPoint = mvpMtx * vec4(vPos + noiseVec*noiseScale, 1.0);
    gl_Position = outputPoint;
    fPos = outputPoint;
}

