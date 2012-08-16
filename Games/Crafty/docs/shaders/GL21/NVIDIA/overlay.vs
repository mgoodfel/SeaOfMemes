#version 120

uniform mat4 mgProjMatrix;
uniform vec2 mgOrigin;

attribute vec2 vertPoint;
attribute vec2 vertTexCoord0;

varying vec2 vTex;

void main(void) 
{ 
  vTex = vertTexCoord0;

  vec4 position;
  position.xy = vertPoint + mgOrigin;
  position.z = 0.0;
  position.w = 1.0;

  gl_Position = mgProjMatrix * position; 
}
