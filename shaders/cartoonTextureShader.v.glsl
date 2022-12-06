#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;
uniform mat4 inverseVPMatrix;


// attribute inputs
in vec3 vPos;
in vec2 vTexCoord;
in vec3 vNormal;

// varying outputs
out vec2 texCoord;
out vec3 fNormal;
out vec4 fPos;

void main() {
  gl_Position = mvpMatrix * vec4(vPos, 1.0);
  fNormal = vec3(normalize(inverseVPMatrix * mvpMatrix * normalize(vec4(vNormal, 0.0))));
  fPos = inverseVPMatrix * mvpMatrix * vec4(vPos, 1.0);

  // TODO #C
  texCoord = vTexCoord;
}
