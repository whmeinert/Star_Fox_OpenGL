#version 410 core

// uniform inputs
// TODO #E
uniform sampler2D textureMap;

// varying inputs
// TODO #D
in vec2 texCoord;

// fragment outputs
out vec4 fragColorOut;

void main() {
  // TODO #F
  vec4 texel = texture(textureMap, texCoord);

  // TODO #G
  fragColorOut = texel;
}