#version 410 core

// in
in vec4 fPos;

out vec4 fragOutColor;

float noise(float pos) {
    float noiseVal = pos * 100;
    noiseVal = noiseVal - floor(noiseVal);
    return noiseVal;
}
void main() {
    vec3 noiseVec = vec3(noise(fPos.x), noise(fPos.y), noise(fPos.z));
    float noiseVal = noiseVec.x;

    fragOutColor = vec4(noiseVal, noiseVal, noiseVal, 1.0);
}

