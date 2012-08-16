#version 120 

uniform mat4 mgModelMatrix;
uniform mat4 mgMVPMatrix;
uniform vec4 mgMatColor;
uniform vec3 mgLightColor;
uniform vec3 mgLightAmbient;

uniform float fogTopHeight;
uniform float fogTopInten;
uniform float fogBotHeight;
uniform float fogBotInten;
uniform float fogMaxDist;

attribute vec2 vertTexCoord0;
attribute vec4 vertPoint;

varying vec4 vFragColor;
varying vec2 vTex;
varying vec3 modelPt;
varying float fogSlope;

void main(void) 
{ 
  vFragColor.rgb = mgMatColor.rgb * (mgLightColor + mgLightAmbient);
  vFragColor.a = mgMatColor.a;
  vTex = vertTexCoord0;

  gl_Position = mgMVPMatrix * vertPoint; 
  modelPt = (mgModelMatrix * vertPoint).xyz;

  fogSlope = (fogTopInten - fogBotInten) / (fogTopHeight - fogBotHeight);
}
