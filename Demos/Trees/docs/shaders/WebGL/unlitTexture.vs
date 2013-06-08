precision mediump float;

uniform mat4 mgMVPMatrix;
uniform mat3 mgNormalMatrix;
uniform vec4 mgMatColor;

attribute vec2 vertTexCoord0;
attribute vec4 vertPoint;
attribute vec3 vertNormal;

varying vec4 vFragColor;
varying vec2 vTex;

void main(void) 
{ 
  vFragColor = mgMatColor;
  vTex = vertTexCoord0;

  gl_Position = mgMVPMatrix * vertPoint; 
}
