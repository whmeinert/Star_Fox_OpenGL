#version 410 core

// uniform inputs
// TODO #E
uniform sampler2D textureMap;
uniform vec3 sunDir;
uniform vec4 sunColor;
uniform vec3 cameraPos;
uniform float doShading;
uniform vec3 laserOnePos;
uniform vec4 laserOneColor;
uniform vec3 laserTwoPos;
uniform vec4 laserTwoColor;
uniform vec3 laserThreePos;
uniform vec4 laserThreeColor;
uniform vec3 laserFourPos;
uniform vec4 laserFourColor;
uniform vec3 laserFivePos;
uniform vec4 laserFiveColor;
uniform vec3 laserSixPos;
uniform vec4 laserSixColor;
uniform vec3 laserSevenPos;
uniform vec4 laserSevenColor;
uniform vec3 laserEightPos;
uniform vec4 laserEightColor;
uniform vec3 laserNinePos;
uniform vec4 laserNineColor;
uniform vec3 laserTenPos;
uniform vec4 laserTenColor;
uniform vec3 laserElevenPos;
uniform vec4 laserElevenColor;
uniform vec3 laserTwelvePos;
uniform vec4 laserTwelveColor;
uniform vec3 laserThirteenPos;
uniform vec4 laserThirteenColor;
uniform vec3 laserFourteenPos;
uniform vec4 laserFourteenColor;
uniform vec3 laserFifteenPos;
uniform vec4 laserFifteenColor;
uniform vec3 laserSixteenPos;
uniform vec4 laserSixteenColor;

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
      vec4 totalI = vec4(0.0);

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
      totalI += sunI;

      vec3 laserAttenuation = vec3(1.0, 0.15, 0.0);

      vec3 laserOneNormal = normalize(fNormal);
      vec3 laserOnel = -normalize(laserOnePos - vec3(fPos));
      vec4 laserOneId = texel * laserOneColor * max(dot(laserOnel, laserOneNormal), 0);
      vec3 laserOner = -laserOnel + 2 * dot(laserOneNormal, laserOnel) * laserOneNormal;
      float laserOneAlpha = 2.0;
      vec4 laserOneIs = texel * laserOneColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), laserOner), 0), laserOneAlpha);
      float laserOneAmbientScale = 0.2;
      vec4 laserOneIa = texel * vec4(laserOneColor.xyz * laserOneAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 laserOneI = laserOneId + laserOneIs + laserOneIa;
      totalI += laserOneI * 1/(laserAttenuation.x + laserAttenuation.y * length(laserOnePos - vec3(fPos)) + laserAttenuation.z * pow(length(laserOnePos - vec3(fPos)),2));

      vec3 laserTwoNormal = normalize(fNormal);
      vec3 laserTwol = -normalize(laserTwoPos - vec3(fPos));
      vec4 laserTwoId = texel * laserTwoColor * max(dot(laserTwol, laserTwoNormal), 0);
      vec3 laserTwor = -laserTwol + 2 * dot(laserTwoNormal, laserTwol) * laserTwoNormal;
      float laserTwoAlpha = 2.0;
      vec4 laserTwoIs = texel * laserTwoColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), laserTwor), 0), laserTwoAlpha);
      float laserTwoAmbientScale = 0.2;
      vec4 laserTwoIa = texel * vec4(laserTwoColor.xyz * laserTwoAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 laserTwoI = laserTwoId + laserTwoIs + laserTwoIa;
      totalI += laserTwoI * 1/(laserAttenuation.x + laserAttenuation.y * length(laserTwoPos - vec3(fPos)) + laserAttenuation.z * pow(length(laserTwoPos - vec3(fPos)),2));

      vec3 laserThreeNormal = normalize(fNormal);
      vec3 laserThreel = -normalize(laserThreePos - vec3(fPos));
      vec4 laserThreeId = texel * laserThreeColor * max(dot(laserThreel, laserThreeNormal), 0);
      vec3 laserThreer = -laserThreel + 2 * dot(laserThreeNormal, laserThreel) * laserThreeNormal;
      float laserThreeAlpha = 2.0;
      vec4 laserThreeIs = texel * laserThreeColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), laserThreer), 0), laserThreeAlpha);
      float laserThreeAmbientScale = 0.2;
      vec4 laserThreeIa = texel * vec4(laserThreeColor.xyz * laserThreeAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 laserThreeI = laserThreeId + laserThreeIs + laserThreeIa;
      totalI += laserThreeI * 1/(laserAttenuation.x + laserAttenuation.y * length(laserThreePos - vec3(fPos)) + laserAttenuation.z * pow(length(laserThreePos - vec3(fPos)),2));

      vec3 laserFourNormal = normalize(fNormal);
      vec3 laserFourl = -normalize(laserFourPos - vec3(fPos));
      vec4 laserFourId = texel * laserFourColor * max(dot(laserFourl, laserFourNormal), 0);
      vec3 laserFourr = -laserFourl + 2 * dot(laserFourNormal, laserFourl) * laserFourNormal;
      float laserFourAlpha = 2.0;
      vec4 laserFourIs = texel * laserFourColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), laserFourr), 0), laserFourAlpha);
      float laserFourAmbientScale = 0.2;
      vec4 laserFourIa = texel * vec4(laserFourColor.xyz * laserFourAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 laserFourI = laserFourId + laserFourIs + laserFourIa;
      totalI += laserFourI * 1/(laserAttenuation.x + laserAttenuation.y * length(laserFourPos - vec3(fPos)) + laserAttenuation.z * pow(length(laserFourPos - vec3(fPos)),2));

      vec3 laserFiveNormal = normalize(fNormal);
      vec3 laserFivel = -normalize(laserFivePos - vec3(fPos));
      vec4 laserFiveId = texel * laserFiveColor * max(dot(laserFivel, laserFiveNormal), 0);
      vec3 laserFiver = -laserFivel + 2 * dot(laserFiveNormal, laserFivel) * laserFiveNormal;
      float laserFiveAlpha = 2.0;
      vec4 laserFiveIs = texel * laserFiveColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), laserFiver), 0), laserFiveAlpha);
      float laserFiveAmbientScale = 0.2;
      vec4 laserFiveIa = texel * vec4(laserFiveColor.xyz * laserFiveAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 laserFiveI = laserFiveId + laserFiveIs + laserFiveIa;
      totalI += laserFiveI * 1/(laserAttenuation.x + laserAttenuation.y * length(laserFivePos - vec3(fPos)) + laserAttenuation.z * pow(length(laserFivePos - vec3(fPos)),2));

      vec3 laserSixNormal = normalize(fNormal);
      vec3 laserSixl = -normalize(laserSixPos - vec3(fPos));
      vec4 laserSixId = texel * laserSixColor * max(dot(laserSixl, laserSixNormal), 0);
      vec3 laserSixr = -laserSixl + 2 * dot(laserSixNormal, laserSixl) * laserSixNormal;
      float laserSixAlpha = 2.0;
      vec4 laserSixIs = texel * laserSixColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), laserSixr), 0), laserSixAlpha);
      float laserSixAmbientScale = 0.2;
      vec4 laserSixIa = texel * vec4(laserSixColor.xyz * laserSixAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 laserSixI = laserSixId + laserSixIs + laserSixIa;
      totalI += laserSixI * 1/(laserAttenuation.x + laserAttenuation.y * length(laserSixPos - vec3(fPos)) + laserAttenuation.z * pow(length(laserSixPos - vec3(fPos)),2));

      vec3 laserSevenNormal = normalize(fNormal);
      vec3 laserSevenl = -normalize(laserSevenPos - vec3(fPos));
      vec4 laserSevenId = texel * laserSevenColor * max(dot(laserSevenl, laserSevenNormal), 0);
      vec3 laserSevenr = -laserSevenl + 2 * dot(laserSevenNormal, laserSevenl) * laserSevenNormal;
      float laserSevenAlpha = 2.0;
      vec4 laserSevenIs = texel * laserSevenColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), laserSevenr), 0), laserSevenAlpha);
      float laserSevenAmbientScale = 0.2;
      vec4 laserSevenIa = texel * vec4(laserSevenColor.xyz * laserSevenAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 laserSevenI = laserSevenId + laserSevenIs + laserSevenIa;
      totalI += laserSevenI * 1/(laserAttenuation.x + laserAttenuation.y * length(laserSevenPos - vec3(fPos)) + laserAttenuation.z * pow(length(laserSevenPos - vec3(fPos)),2));

      vec3 laserEightNormal = normalize(fNormal);
      vec3 laserEightl = -normalize(laserEightPos - vec3(fPos));
      vec4 laserEightId = texel * laserEightColor * max(dot(laserEightl, laserEightNormal), 0);
      vec3 laserEightr = -laserEightl + 2 * dot(laserEightNormal, laserEightl) * laserEightNormal;
      float laserEightAlpha = 2.0;
      vec4 laserEightIs = texel * laserEightColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), laserEightr), 0), laserEightAlpha);
      float laserEightAmbientScale = 0.2;
      vec4 laserEightIa = texel * vec4(laserEightColor.xyz * laserEightAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 laserEightI = laserEightId + laserEightIs + laserEightIa;
      totalI += laserEightI * 1/(laserAttenuation.x + laserAttenuation.y * length(laserEightPos - vec3(fPos)) + laserAttenuation.z * pow(length(laserEightPos - vec3(fPos)),2));

      vec3 laserNineNormal = normalize(fNormal);
      vec3 laserNinel = -normalize(laserNinePos - vec3(fPos));
      vec4 laserNineId = texel * laserNineColor * max(dot(laserNinel, laserNineNormal), 0);
      vec3 laserNiner = -laserNinel + 2 * dot(laserNineNormal, laserNinel) * laserNineNormal;
      float laserNineAlpha = 2.0;
      vec4 laserNineIs = texel * laserNineColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), laserNiner), 0), laserNineAlpha);
      float laserNineAmbientScale = 0.2;
      vec4 laserNineIa = texel * vec4(laserNineColor.xyz * laserNineAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 laserNineI = laserNineId + laserNineIs + laserNineIa;
      totalI += laserNineI * 1/(laserAttenuation.x + laserAttenuation.y * length(laserNinePos - vec3(fPos)) + laserAttenuation.z * pow(length(laserNinePos - vec3(fPos)),2));

      vec3 laserTenNormal = normalize(fNormal);
      vec3 laserTenl = -normalize(laserTenPos - vec3(fPos));
      vec4 laserTenId = texel * laserTenColor * max(dot(laserTenl, laserTenNormal), 0);
      vec3 laserTenr = -laserTenl + 2 * dot(laserTenNormal, laserTenl) * laserTenNormal;
      float laserTenAlpha = 2.0;
      vec4 laserTenIs = texel * laserTenColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), laserTenr), 0), laserTenAlpha);
      float laserTenAmbientScale = 0.2;
      vec4 laserTenIa = texel * vec4(laserTenColor.xyz * laserTenAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 laserTenI = laserTenId + laserTenIs + laserTenIa;
      totalI += laserTenI * 1/(laserAttenuation.x + laserAttenuation.y * length(laserTenPos - vec3(fPos)) + laserAttenuation.z * pow(length(laserTenPos - vec3(fPos)),2));

      vec3 laserElevenNormal = normalize(fNormal);
      vec3 laserElevenl = -normalize(laserElevenPos - vec3(fPos));
      vec4 laserElevenId = texel * laserElevenColor * max(dot(laserElevenl, laserElevenNormal), 0);
      vec3 laserElevenr = -laserElevenl + 2 * dot(laserElevenNormal, laserElevenl) * laserElevenNormal;
      float laserElevenAlpha = 2.0;
      vec4 laserElevenIs = texel * laserElevenColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), laserElevenr), 0), laserElevenAlpha);
      float laserElevenAmbientScale = 0.2;
      vec4 laserElevenIa = texel * vec4(laserElevenColor.xyz * laserElevenAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 laserElevenI = laserElevenId + laserElevenIs + laserElevenIa;
      totalI += laserElevenI * 1/(laserAttenuation.x + laserAttenuation.y * length(laserElevenPos - vec3(fPos)) + laserAttenuation.z * pow(length(laserElevenPos - vec3(fPos)),2));

      vec3 laserTwelveNormal = normalize(fNormal);
      vec3 laserTwelvel = -normalize(laserTwelvePos - vec3(fPos));
      vec4 laserTwelveId = texel * laserTwelveColor * max(dot(laserTwelvel, laserTwelveNormal), 0);
      vec3 laserTwelver = -laserTwelvel + 2 * dot(laserTwelveNormal, laserTwelvel) * laserTwelveNormal;
      float laserTwelveAlpha = 2.0;
      vec4 laserTwelveIs = texel * laserTwelveColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), laserTwelver), 0), laserTwelveAlpha);
      float laserTwelveAmbientScale = 0.2;
      vec4 laserTwelveIa = texel * vec4(laserTwelveColor.xyz * laserTwelveAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 laserTwelveI = laserTwelveId + laserTwelveIs + laserTwelveIa;
      totalI += laserTwelveI * 1/(laserAttenuation.x + laserAttenuation.y * length(laserTwelvePos - vec3(fPos)) + laserAttenuation.z * pow(length(laserTwelvePos - vec3(fPos)),2));

      vec3 laserThirteenNormal = normalize(fNormal);
      vec3 laserThirteenl = -normalize(laserThirteenPos - vec3(fPos));
      vec4 laserThirteenId = texel * laserThirteenColor * max(dot(laserThirteenl, laserThirteenNormal), 0);
      vec3 laserThirteenr = -laserThirteenl + 2 * dot(laserThirteenNormal, laserThirteenl) * laserThirteenNormal;
      float laserThirteenAlpha = 2.0;
      vec4 laserThirteenIs = texel * laserThirteenColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), laserThirteenr), 0), laserThirteenAlpha);
      float laserThirteenAmbientScale = 0.2;
      vec4 laserThirteenIa = texel * vec4(laserThirteenColor.xyz * laserThirteenAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 laserThirteenI = laserThirteenId + laserThirteenIs + laserThirteenIa;
      totalI += laserThirteenI * 1/(laserAttenuation.x + laserAttenuation.y * length(laserThirteenPos - vec3(fPos)) + laserAttenuation.z * pow(length(laserThirteenPos - vec3(fPos)),2));

      vec3 laserFourteenNormal = normalize(fNormal);
      vec3 laserFourteenl = -normalize(laserFourteenPos - vec3(fPos));
      vec4 laserFourteenId = texel * laserFourteenColor * max(dot(laserFourteenl, laserFourteenNormal), 0);
      vec3 laserFourteenr = -laserFourteenl + 2 * dot(laserFourteenNormal, laserFourteenl) * laserFourteenNormal;
      float laserFourteenAlpha = 2.0;
      vec4 laserFourteenIs = texel * laserFourteenColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), laserFourteenr), 0), laserFourteenAlpha);
      float laserFourteenAmbientScale = 0.2;
      vec4 laserFourteenIa = texel * vec4(laserFourteenColor.xyz * laserFourteenAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 laserFourteenI = laserFourteenId + laserFourteenIs + laserFourteenIa;
      totalI += laserFourteenI * 1/(laserAttenuation.x + laserAttenuation.y * length(laserFourteenPos - vec3(fPos)) + laserAttenuation.z * pow(length(laserFourteenPos - vec3(fPos)),2));

      vec3 laserFifteenNormal = normalize(fNormal);
      vec3 laserFifteenl = -normalize(laserFifteenPos - vec3(fPos));
      vec4 laserFifteenId = texel * laserFifteenColor * max(dot(laserFifteenl, laserFifteenNormal), 0);
      vec3 laserFifteenr = -laserFifteenl + 2 * dot(laserFifteenNormal, laserFifteenl) * laserFifteenNormal;
      float laserFifteenAlpha = 2.0;
      vec4 laserFifteenIs = texel * laserFifteenColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), laserFifteenr), 0), laserFifteenAlpha);
      float laserFifteenAmbientScale = 0.2;
      vec4 laserFifteenIa = texel * vec4(laserFifteenColor.xyz * laserFifteenAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 laserFifteenI = laserFifteenId + laserFifteenIs + laserFifteenIa;
      totalI += laserFifteenI * 1/(laserAttenuation.x + laserAttenuation.y * length(laserFifteenPos - vec3(fPos)) + laserAttenuation.z * pow(length(laserFifteenPos - vec3(fPos)),2));

      vec3 laserSixteenNormal = normalize(fNormal);
      vec3 laserSixteenl = -normalize(laserSixteenPos - vec3(fPos));
      vec4 laserSixteenId = texel * laserSixteenColor * max(dot(laserSixteenl, laserSixteenNormal), 0);
      vec3 laserSixteenr = -laserSixteenl + 2 * dot(laserSixteenNormal, laserSixteenl) * laserSixteenNormal;
      float laserSixteenAlpha = 2.0;
      vec4 laserSixteenIs = texel * laserSixteenColor * pow(max(dot(normalize(cameraPos - vec3(fPos)), laserSixteenr), 0), laserSixteenAlpha);
      float laserSixteenAmbientScale = 0.2;
      vec4 laserSixteenIa = texel * vec4(laserSixteenColor.xyz * laserSixteenAmbientScale, 1.0);
      // vec4 sunI = sunId + sunIs + sunIa;
      vec4 laserSixteenI = laserSixteenId + laserSixteenIs + laserSixteenIa;
      totalI += laserSixteenI * 1/(laserAttenuation.x + laserAttenuation.y * length(laserSixteenPos - vec3(fPos)) + laserAttenuation.z * pow(length(laserSixteenPos - vec3(fPos)),2));


      r = totalI.x;
      g = totalI.y;
      b = totalI.z;
      a = totalI.w;
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
