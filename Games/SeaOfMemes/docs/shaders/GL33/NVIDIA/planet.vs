#version 330 

uniform mat4 mgMVPMatrix;

in vec4 vertPoint;
in vec3 modelPoint;

smooth out vec3 vFragPt;

void main(void) 
{
  vFragPt = modelPoint;

  gl_Position = mgMVPMatrix * vertPoint; 
}
