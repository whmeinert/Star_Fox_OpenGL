#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;

// attribute inputs
in vec3 vPos;
in vec2 vTexCoord;

// varying outputs
out vec2 texCoord;

void main() {
  gl_Position = mvpMatrix * vec4(vPos, 1.0);

  // TODO #C
  texCoord = vTexCoord;
}