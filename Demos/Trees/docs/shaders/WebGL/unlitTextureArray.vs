precision mediump float;

uniform mat4 mgMVPMatrix;
uniform mat3 mgNormalMatrix;
uniform vec4 mgMatColor;
uniform vec2 mgAtlasSize;
uniform vec2 mgTextureSize;

attribute vec3 vertTexCoord0;
attribute vec4 vertPoint;
attribute vec3 vertNormal;

varying vec4 vFragColor;
varying vec4 vTex;
varying vec4 vTexOrigin;
varying vec4 vTexSize;

void main(void) 
{ 
  vTex.xy = vertTexCoord0.xy;
  vTexSize.x = mgTextureSize.x / mgAtlasSize.x;
  vTexSize.y = mgTextureSize.y / mgAtlasSize.y;

  float cols = floor(mgAtlasSize.x / mgTextureSize.x);
  float cell = floor(vertTexCoord0.z);
  vTexOrigin.x = mod(cell, cols) * mgTextureSize.x;
  vTexOrigin.y = floor(cell/cols) * mgTextureSize.y;
  vTexOrigin.x /= mgAtlasSize.x;
  vTexOrigin.y /= mgAtlasSize.y;

  vFragColor = mgMatColor;

  gl_Position = mgMVPMatrix * vertPoint; 
}
