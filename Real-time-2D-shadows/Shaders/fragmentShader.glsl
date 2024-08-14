#version 300 es
precision mediump float;

uniform sampler2D height;
uniform sampler2D voronoiTex;
uniform sampler2D randomNoise;

uniform float heightMod;
uniform float millis;
uniform float waterLevel;
uniform vec3 sunPos;
uniform vec3 pix; // normalised size of a pixel, zero in z
uniform vec2 res;
uniform mat2 rot;

const float slopeCutoff = 0.3;
const float heightCutoff = 0.5;
const float terrainVariation = 0.005;

// Terrain colours
const vec3 colourDeepWater = vec3(0.,0.431,0.694);
const vec3 colourWater = vec3(0.384,0.651,0.663);
const vec3 colourSand = vec3(0.839,0.714,0.62);
const vec3 colourGrass = vec3(0.596,0.678,0.353);
const vec3 colourBush = vec3(0.396,0.522,0.255);
const vec3 colourForest = vec3(0.278,0.463,0.271);
const vec3 colourStone = vec3(0.427,0.463,0.529);
const vec3 colourSlate = vec3(0.518,0.553,0.604);
const vec3 colourSnow = vec3(0.824,0.878,0.871);

const vec3 shadowBrightness = vec3(0.45, 0.45, 0.5);

const vec3 viewPos = vec3(0.5, 0.5, 2.);

const int MAX_STEPS = 300;

in vec2 pos;

float unpackHeight(vec4 h){
    return (h.r + h.g + h.b + h.a) / 4.;
}

// use the four neighbouring pixels and
// lerp the height
float getHeightRaw(vec2 pos) {
  
  vec2 res = 1./pix.xy;
  
  vec2 p = pos * res;
  
  vec2 lerpP = fract(p);
  
  p = floor(p);
  
  p *= pix.xy;
  
  float tl = unpackHeight(texture(height, p));
  float bl = unpackHeight(texture(height, p + pix.zy));
  
  float tr = unpackHeight(texture(height, p + pix.xz));
  float br = unpackHeight(texture(height, p + pix.xy));
  
  float t = mix(tl, tr, lerpP.x);
  float b = mix(bl, br, lerpP.x);
  
  return mix(t, b, lerpP.y) * heightMod;
}

float getWaterNoise(vec2 pos) {
  // Reduce the scale factor directly in the texture lookup
  vec2 uv = pos * res * 8.;
  uv = uv - floor(uv / vec2(textureSize(voronoiTex, 0))) * vec2(textureSize(voronoiTex, 0));
  return 0.003 * (texture(voronoiTex, uv).r * 2. - 1.);
}

float getWaterLevel(vec2 pos) {
  float t = millis / 100000.;
  
  // Simplified to reduce the effect of 't' moving the position around, thus reusing cached texture fetches more effectively
  vec2 offsetPos = pos + vec2(t, t); // Use uniform motion to reduce complexity
  float waveHeight = getWaterNoise(offsetPos);
  waveHeight += getWaterNoise(pos * rot - vec2(t, 0.));

  // Remove additional rotation and use only one water noise sample per call
  return waterLevel + waveHeight;
}

float getHeight(vec2 pos) {
  // Direct call to avoid unnecessary max operation if not needed
  float terrainHeight = getHeightRaw(pos);
  float waterHeight = getWaterLevel(pos);
  return terrainHeight > waterHeight ? terrainHeight : waterHeight;
}

float getHeightNoise(vec2 p) {
  vec2 uv = pos * res * 8.;
  uv = uv - floor(uv / vec2(textureSize(randomNoise, 0))) * vec2(textureSize(randomNoise, 0));
  return (texture(randomNoise, uv).r * 2. - 1.) * 1.5;
}

vec3 getTerrainColour(float h, vec3 normal) {

  if(h > 0.39){
    // add some random variation around the bands;
    h += (getHeightNoise(pos/pix.xy) * 2. - 1.) * terrainVariation;
  }

  float flatness = dot(normal, vec3(0., 0., 1.));
  float isSteep = 1. - step(slopeCutoff, flatness);
  float isHigh = step(heightCutoff, h);
  
  float forceStone = isSteep * isHigh; //must be steep and high
  
  vec3 col = colourSnow;

  if(h < waterLevel){
    col = colourWater;
  } else if(h < 0.4) {
    col = colourSand;
  } else if(h < 0.5) {
    col = colourGrass;
  } else if(h < 0.63) {
    col = colourBush;
  } else if(h < 0.75) {
    col = colourForest;
  } else if(h < 0.8) {
    col = colourStone;
  } else if(h < 0.9) {
    col = colourSlate;
  }

  return col * (1. - forceStone) + colourStone * forceStone;
}

vec3 getNormal(vec2 pos) {
  float h = 200.;
  float l = h * getHeight(pos - pix.xz);
  float r = h * getHeight(pos + pix.xz);
  
  float d = h * getHeight(pos - pix.zy);
  float u = h * getHeight(pos + pix.zy);
  
  return normalize(vec3(l-r, d-u, 1.));
}

float easeOut(float x) {
  return clamp(1. - pow(1. - x, 6.), 0., 1.);
}

void main() {
  // Get height from height map
  float hgt = getHeightRaw(pos);
  
  // Starting point for ray cast
  vec3 op = vec3(pos, getHeight(pos));
  vec3 p = op;
  
  float minStepSize = min(pix.x, pix.y);
  
  // Direction of raycast is towards the sun from centre of screen
  vec3 sunDir = sunPos - vec3(0.5, 0.5, 0.);
  vec3 stepDir = normalize(sunDir);
    
  float inShadow = 0.;
  int n = 0;
  for(int i = 1; i <= MAX_STEPS; i ++) {    
    n++;
    // Check height at new location
    float h = getHeight(p.xy);
    if(h > p.z) {
      // ray is inside the terrain
      // therefore must be in shadow
      inShadow = 1.;
      break;
    }
    if(p.z > 1.) {
      // above the heighest terrain level
      // will not be in shadow
      break;
    }
    
    // Step towards the sun by dist to heightmap
    p += stepDir * max(minStepSize, (p.z - h) * 0.02);
  }
  if(n == MAX_STEPS) {
    inShadow = 1.;
  }

  // calculate normal based on surrounding tiles
  vec3 normal = getNormal(pos);
  float normalShadow = (dot(normal, -stepDir) + 1.) * 0.5;
  
  
  vec3 col = getTerrainColour(hgt, normal);
  
  float terrainHeight = getHeightRaw(pos);
  float waterHeight = getWaterLevel(pos);
  float waterDepth =  waterHeight - terrainHeight;

  // Add water if the terrain is below water level
  float isWater = step(0., waterDepth);
  // Add waves if the water depth is really shallow
  float isWave = isWater * (1. - smoothstep(0., 0.02, waterDepth));
  isWave *= (sin(cos(millis/500.)*2. + waterDepth * 500.) + 1.) * 2.;
  
  float waterLerp = easeOut(waterDepth/waterHeight);
  
  vec3 waterColour = mix(colourWater, colourDeepWater, easeOut(waterDepth/waterHeight));
  
  waterColour = mix(waterColour, vec3(1.), isWave);
  
  // Simulate see-through water by mixing between the terrain and water
  // colour based on depth
  col *= mix(col, waterColour, waterLerp);
  
  // Add normal shadows into shadow variable
  inShadow = clamp(inShadow + normalShadow, 0., 1.);
  
  // Create a darker version of the colour for the shadows (blue shifted);
  vec3 shadowCol = col * shadowBrightness * vec3(1., 1., 1. + inShadow * 0.2);  

  vec3 viewDir = normalize(viewPos - op);
  vec3 halfwayDir = normalize(sunDir + viewDir);

  float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.);
  vec3 specular = (1. - inShadow) * isWater * 0.4 * spec * vec3(1., 1., 1.);
  
  gl_FragColor = mix(vec4(col, 1.) + (vec4(specular, 1.) * 0.05), vec4(shadowCol, 1.), inShadow);
}


