#version 330

uniform mat4 mgMVPMatrix;
uniform mat3 mgNormalMatrix;
uniform vec4 mgMatColor;

in vec2 vertTexCoord0;
in vec4 vertPoint;
in vec3 vertNormal;

smooth out vec4 vFragColor;
smooth out vec2 vTex;

void main(void) 
{ 
  vFragColor = mgMatColor;
  vTex = vertTexCoord0;

  gl_Position = mgMVPMatrix * vertPoint; 
}
