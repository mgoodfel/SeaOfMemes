precision mediump float;

uniform mat4 mgMVPMatrix;
uniform mat3 mgNormalMatrix;
uniform vec4 mgMatColor;

attribute vec3 vertTexCoord0;
attribute vec4 vertPoint;
attribute vec3 vertNormal;

varying vec4 vFragColor;
varying vec3 vTex;

void main(void) 
{ 
  vFragColor = mgMatColor;
  vTex = vertTexCoord0;

  gl_Position = mgMVPMatrix * vertPoint; 
}
