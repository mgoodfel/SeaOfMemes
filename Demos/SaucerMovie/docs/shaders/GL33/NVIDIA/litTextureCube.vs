#version 330 

uniform mat4 mgMVPMatrix;
uniform mat3 mgNormalMatrix;
uniform vec3 mgLightDir;
uniform vec3 mgLightColor;
uniform vec3 mgLightAmbient;
uniform vec4 mgMatColor;

in vec4 vertPoint;
in vec3 vertNormal;
in vec3 vertTexCoord0;

out vec4 vFragColor;
smooth out vec3 vTex;

void main(void) 
{ 
  vec3 eyeNormal = mgNormalMatrix * vertNormal;
  float lightInten = max(0.0, dot(eyeNormal, mgLightDir));

  vFragColor.rgb = mgMatColor.rgb * (mgLightColor * lightInten + mgLightAmbient);
  vFragColor.a = mgMatColor.a;
  vTex = vertTexCoord0;

  gl_Position = mgMVPMatrix * vertPoint; 
}
