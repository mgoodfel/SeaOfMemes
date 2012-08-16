#version 120

uniform mat4 worldMatrix;
uniform mat3 normalMatrix;
uniform mat4 projMatrix;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 lightAmbient;
uniform vec4 matColor;

uniform vec2 vAtlasOrigins[50];
uniform vec2 vAtlasSizes[50];

attribute vec4 vVertex;
attribute vec3 vNormal;
attribute vec3 vTexCoord0;

varying vec2 vTextureOrigin;
varying vec2 vTextureSize;
varying vec4 vFragColor;
varying vec2 vTex;

void main(void) 
{ 
  vec3 eyeNormal = normalMatrix * vNormal;
  float lightInten = max(0.0, dot(eyeNormal, lightDir));

  vFragColor.rgb = matColor.rgb * lightColor * lightInten + lightAmbient;
  vFragColor.a = matColor.a;
  vTex = vTexCoord0.xy;
  int index = int(vTexCoord0.z);
  vTextureOrigin = vAtlasOrigins[index];
  vTextureSize = vAtlasSizes[index];

  mat4 mvpMatrix;
  mvpMatrix = projMatrix * worldMatrix;
  gl_Position = mvpMatrix * vVertex; 
}
