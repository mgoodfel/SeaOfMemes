#version 120

uniform mat4 mgMVPMatrix;
uniform mat3 mgNormalMatrix;
uniform vec3 mgLightDir;
uniform vec3 mgLightColor;
uniform vec3 mgLightAmbient;
uniform vec4 mgMatColor;

attribute vec2 vertTexCoord0;
attribute vec4 vertPoint;
attribute vec3 vertNormal;

varying vec4 vFragColor;
varying vec2 vTex;

void main(void) 
{
  vec3 eyeNormal = mgNormalMatrix * vertNormal;
  float lightInten = max(0.0, dot(eyeNormal, mgLightDir));

  vFragColor.rgb = mgMatColor.rgb * (mgLightColor * lightInten + mgLightAmbient);
  vFragColor.a = mgMatColor.a;

  vTex = vertTexCoord0;

  gl_Position = mgMVPMatrix * vertPoint; 
}
