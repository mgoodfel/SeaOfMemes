#version 330 

uniform sampler2DArray mgTextureUnit0;

in vec4 vFragColor;
smooth in vec3 vTex;

out vec4 outFragColor;

void main(void) 
{ 
  outFragColor = vFragColor * texture(mgTextureUnit0, vTex);
}
