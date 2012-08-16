#version 330 

uniform mat4 mgMVPMatrix;
uniform mat3 mgNormalMatrix;
uniform vec3 mgLightDir;
uniform vec3 mgLightColor;
uniform vec3 mgLightAmbient;
uniform vec4 mgMatColor;

in vec3 vertPoint;
in vec3 vertNormal;
in vec2 vertTexCoord0;
in ivec4 vertColor;

smooth out vec4 vFragColor;
smooth out vec2 vTex;

void main(void) 
{ 
  vec3 eyeNormal = mgNormalMatrix * vertNormal;
  float lightInten = max(0.0, dot(eyeNormal, mgLightDir));

  vec4 vInColor;
  vInColor = vertColor/255.0;

  vFragColor.rgb = vInColor.rgb * (mgLightColor * lightInten + mgLightAmbient);
  vFragColor.a = vInColor.a;

  vTex = vertTexCoord0;

  vec4 pt;
  pt.xyz = vertPoint;
  pt.w = 1.0;
  gl_Position = mgMVPMatrix * pt; 
}
