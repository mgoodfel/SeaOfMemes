#version 120 

uniform mat4 mgMVPMatrix;
uniform mat3 mgNormalMatrix;
uniform vec3 mgLightDir;
uniform vec3 mgLightColor;
uniform vec3 mgLightAmbient;
uniform vec4 mgMatColor;

attribute vec4 vertPoint;
attribute vec3 vertNormal;
attribute vec2 vertTexCoord0;
attribute vec4 vertColor;

varying vec4 vFragColor;
varying vec2 vTex;

void main(void) 
{ 
  vec3 eyeNormal = mgNormalMatrix * vertNormal;
  float lightInten = max(0.0, dot(eyeNormal, mgLightDir));

  vec4 vInColor;
  vInColor = vertColor/255.0;

  vFragColor.rgb = vInColor.rgb * (mgLightColor * lightInten + mgLightAmbient);
  vFragColor.a = vInColor.a;

  vTex = vertTexCoord0;

  gl_Position = mgMVPMatrix * vertPoint; 
}
