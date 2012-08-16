#version 330 

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

in vec2 vertTexCoord0;
in vec4 vertPoint;

smooth out vec4 vFragColor;
smooth out vec2 vTex;
smooth out vec3 modelPt;
flat out float fogSlope;

void main(void) 
{ 
  vFragColor.rgb = mgMatColor.rgb * (mgLightColor + mgLightAmbient);
  vFragColor.a = mgMatColor.a;
  vTex = vertTexCoord0;

  gl_Position = mgMVPMatrix * vertPoint; 
  modelPt = (mgModelMatrix * vertPoint).xyz;

  fogSlope = (fogTopInten - fogBotInten) / (fogTopHeight - fogBotHeight);
}
