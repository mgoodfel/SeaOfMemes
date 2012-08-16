#version 330

uniform mat4 mgMVPMatrix;
uniform mat3 mgNormalMatrix;
uniform vec3 mgLightColor;
uniform vec3 mgLightAmbient;
uniform vec4 mgMatColor;

uniform vec3 lightPosn;

in vec2 vertTexCoord0;
in vec4 vertPoint;
in vec3 vertNormal;

out vec4 vFragColor;
smooth out vec2 vTex;

void main(void) 
{ 
  vec3 lightDir = normalize(lightPosn - vertPoint.xyz);
  vec3 eyeNormal = mgNormalMatrix * vertNormal;
  float lightInten = max(0.0, dot(vertNormal, lightDir));

  vFragColor.rgb = mgMatColor.rgb * (mgLightColor * lightInten + mgLightAmbient);
  vFragColor.a = mgMatColor.a;
  vTex = vertTexCoord0;

  gl_Position = mgMVPMatrix * vertPoint; 
}
