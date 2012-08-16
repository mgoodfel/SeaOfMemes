#version 330

uniform sampler2D mgTextureUnit0;

smooth in vec2 vTex;
out vec4 outFragColor;

void main(void) 
{ 
  outFragColor = texture(mgTextureUnit0, vTex);
}
