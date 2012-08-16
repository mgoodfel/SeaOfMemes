#version 120 

uniform mat4 mgMVPMatrix;
uniform mat3 mgNormalMatrix;
uniform vec3 mgLightDir;
uniform vec3 mgLightColor;
uniform vec3 mgLightAmbient;
uniform vec4 mgMatColor;
uniform vec2 mgAtlasSize;
uniform vec2 mgTextureSize;

attribute vec4 vertPoint;
attribute vec3 vertNormal;
attribute vec3 vertTexCoord0;

varying vec4 vFragColor;
varying vec4 vTex;
varying vec4 vTexOrigin;
varying vec4 vTexSize;

void main(void) 
{ 
  vTex.xy = vertTexCoord0.xy;
  vTexSize.x = mgTextureSize.x/mgAtlasSize.x;
  vTexSize.y = mgTextureSize.y/mgAtlasSize.y;
  
  float cols = floor(mgAtlasSize.x / mgTextureSize.x);
  float cell = floor(vertTexCoord0.z);
  vTexOrigin.x = mod(cell, cols) * mgTextureSize.x;
  vTexOrigin.y = floor(cell/cols) * mgTextureSize.y;
  vTexOrigin.x /= mgAtlasSize.x;
  vTexOrigin.y /= mgAtlasSize.y;
  
  vec3 eyeNormal = mgNormalMatrix * vertNormal;
  float lightInten = max(0.0, dot(eyeNormal, mgLightDir));

  vFragColor.rgb = mgMatColor.rgb * (mgLightColor * lightInten + mgLightAmbient);
  vFragColor.a = mgMatColor.a;

  gl_Position = mgMVPMatrix * vertPoint; 
}
