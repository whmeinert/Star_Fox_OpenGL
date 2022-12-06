#version 410 core

// uniform inputs
// TODO #E
uniform sampler2D textureMap;

// varying inputs
in vec2 texCoord;

// fragment outputs
out vec4 fragColorOut;

void main() {
  // TODO #F
  vec4 texel = texture(textureMap, texCoord);

  // Attempt cartoon shader
  float r = texel.x;
  float g = texel.y;
  float b = texel.z;
  float a = texel.w;

  // convert to hsv
  // Using https://www.geeksforgeeks.org/program-change-rgb-color-model-hsv-color-model/
  float cmax = max(r, max(g, b));
  float cmin = min(r, min(g, b));
  float diff = cmax - cmin;
  float h = -1;
  float s = -1;

  if(cmax == cmin) {
      h = 0;
  } else if(cmax == r) {
      //h = mod((60 * ((g - b) / diff) + 360), 360);
      h = 60 * mod((g-b) / diff, 6);
  } else if(cmax == g) {
      //h = mod((60 * ((b - r) / diff) + 120), 360);
      h = 60 * ((b-r) / diff + 2);
  } else {
      // h = mod((60 * ((r - g) / diff) + 240), 360);
      h = 60 * ((r-g) / diff + 4);
  }

  if(cmax == 0) {
      s = 0;
  } else {
      s = (diff / cmax);
  }

  float v = cmax;


  float hSplits = 8.0;
  float hSplitAmt = 360.0/(hSplits - 1.0) / 2.0;
  for(float split = 0; split <= 360.0; split += hSplitAmt*2.0) {
     if(h < split + hSplitAmt) {
         h = split;
         break;
     }
  }

  float sSplits = 4.0;
  float sSplitAmt = 1.0/(sSplits - 1.0) / 2.0;
  for(float split = 0; split <= 1.0; split += sSplitAmt*2.0) {
     if(s < split + sSplitAmt) {
         s = split;
         break;
     }
  }

  float vSplits = 4.0;
  float vSplitAmt = 1.0/(vSplits - 1.0) / 2.0;
  for(float split = 0; split <= 1.0; split += vSplitAmt*2.0) {
     if(v < split + vSplitAmt) {
         v = split;
         break;
     }
  }

  // convert back to rgb
  float newR = -1;
  float newG = -1;
  float newB = -1;
  float newA = a;

  float C = v*s;
  float X = C*(1-abs(mod(h/60, 2)-1));
  float m = v-C;

  if(h < 60) {
      newR = C;
      newG = X;
      newB = 0;
  } else if(h < 120) {
      newR = X;
      newG = C;
      newB = 0;
  } else if(h < 180) {
      newR = 0;
      newG = C;
      newB = X;
  } else if(h < 240) {
      newR = 0;
      newG = X;
      newB = C;
  } else if(h < 300) {
      newR = X;
      newG = 0;
      newB = C;
  } else {
      newR = C;
      newG = 0;
      newB = X;
  }

  newR += m;
  newG += m;
  newB += m;


  // TODO #G
  fragColorOut = vec4(newR, newG, newB, newA);
}
