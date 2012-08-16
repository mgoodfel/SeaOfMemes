#version 330 

uniform mat4 mgMVPMatrix;
uniform mat3 mgNormalMatrix;
uniform vec4 mgMatColor;

in vec3 vertTexCoord0;
in vec4 vertPoint;
in vec3 vertNormal;

out vec4 vFragColor;
smooth out vec3 vTex;

void main(void) 
{ 
  vFragColor = mgMatColor;
  vTex = vertTexCoord0;

  gl_Position = mgMVPMatrix * vertPoint; 
}
