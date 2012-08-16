#version 120

uniform mat4 mgMVPMatrix;
uniform mat3 mgNormalMatrix;
uniform vec3 mgLightDir;
uniform vec3 mgLightColor;
uniform vec3 mgLightAmbient;
uniform vec4 mgMatColor;
uniform vec2 mgAtlasSize;
uniform vec2 mgTextureSize;

uniform float textureCycle;

attribute vec2 vertTexCoord0;
attribute vec4 vertPoint;
attribute vec3 vertNormal;

varying vec4 vFragColor;
varying vec4 vTexSize;
varying vec4 vTex0;
varying vec4 vTexOrigin0;
varying vec4 vTex1;
varying vec4 vTexOrigin1;
varying float blend;

void main(void) 
{ 
  vec3 eyeNormal = mgNormalMatrix * vertNormal;
  float lightInten = max(0.0, dot(eyeNormal, mgLightDir));

  vFragColor.rgb = mgMatColor.rgb * (mgLightColor * lightInten + mgLightAmbient);
  vFragColor.a = mgMatColor.a;

  float cols = floor(mgAtlasSize.x / mgTextureSize.x);
  vTexSize.x = mgTextureSize.x/mgAtlasSize.x;
  vTexSize.y = mgTextureSize.x/mgAtlasSize.y;

  float cell;

  vTex0.xy = vertTexCoord0;
  vTex1 = vTex0;
  
  if (textureCycle >= 0)
  {
    // blend between two animation textures
    cell = floor(textureCycle);
    blend = textureCycle - cell;

    vTexOrigin0.x = mod(cell, cols) * mgTextureSize.x;
    vTexOrigin0.y = floor(cell/cols) * mgTextureSize.y;
    vTexOrigin0.x /= mgAtlasSize.x;
    vTexOrigin0.y /= mgAtlasSize.y;

    cell = mod(1+cell, 10.0);

    vTexOrigin1.x = mod(cell, cols) * mgTextureSize.x;
    vTexOrigin1.y = floor(cell/cols) * mgTextureSize.y;
    vTexOrigin1.x /= mgAtlasSize.x;
    vTexOrigin1.y /= mgAtlasSize.y;
  }
  else
  {
    // turn animation off
    cell = 10;
    blend = 0.0;

    vTexOrigin0.x = mod(cell, cols) * mgTextureSize.x;
    vTexOrigin0.y = floor(cell/cols) * mgTextureSize.y;
    vTexOrigin0.x /= mgAtlasSize.x;
    vTexOrigin0.y /= mgAtlasSize.y;
    
    vTexOrigin1 = vTexOrigin0;
  }

  gl_Position = mgMVPMatrix * vertPoint; 
}
