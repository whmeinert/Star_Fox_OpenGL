#version 410 core

// uniform inputs
// TODO #E
uniform sampler2D textureMap;
uniform vec3 sunDir;
uniform vec4 sunColor;
uniform vec3 cameraPos;
uniform float doShading;

// varying inputs
in vec2 texCoord;
in vec3 fNormal;
in vec4 fPos;


// fragment outputs
out vec4 fragColorOut;

void main() {
  // TODO #F
  vec4 texel = texture(textureMap, texCoord);

  // Phong shader
  float r = texel.x;
  float g = texel.y;
  float b = texel.z;
  float a = texel.w;
  if(doShading > 0.5) {
      vec3 sunNormal = normalize(fNormal);
      vec3 sunl = -normalize(sunDir);
      vec4 sunId = texel * sunColor * max(dot(sunl, sunNormal), 0);
      vec3 sunr = -sunl + 2 * dot(sunNormal, sunl) * sunNormal;
      float sunAlpha = 2.0;
      vec4 sunIs = texel * sunColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), sunr), 0), sunAlpha);
      float sunAmbientScale = 0.4;
      vec4 sunIa = texel * vec4(sunColor.xyz * sunAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 sunI = sunId + sunIs + sunIa;

      r = sunI.x;
      g = sunI.y;
      b = sunI.z;
      a = sunI.w;
  }

  // Attempt cartoon shader
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
