#version 120

uniform sampler2D mgTextureUnit0;

varying vec2 vTex;

void main(void) 
{ 
  gl_FragColor = texture2D(mgTextureUnit0, vTex);
}
