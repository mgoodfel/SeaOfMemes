#version 330

uniform mat4 mgMVPMatrix;
uniform mat3 mgNormalMatrix;
uniform vec4 mgMatColor;

in vec3 vertPoint;
in vec3 vertNormal;
in vec2 vertTexCoord0;

out vec4 vFragColor;
smooth out vec2 vTex;

void main(void) 
{ 
  vFragColor = mgMatColor;
  vTex = vertTexCoord0;

  vec4 pt;
  pt.xyz = vertPoint.xyz;
  pt.w = 1.0;
  gl_Position = mgMVPMatrix * pt; 
}
