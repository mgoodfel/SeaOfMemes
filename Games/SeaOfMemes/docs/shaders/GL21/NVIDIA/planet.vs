#version 120 

uniform mat4 mgMVPMatrix;

attribute vec4 vertPoint;
attribute vec3 modelPoint;

varying vec3 vFragPt;

void main(void) 
{
  vFragPt = modelPoint;

  gl_Position = mgMVPMatrix * vertPoint; 
}
