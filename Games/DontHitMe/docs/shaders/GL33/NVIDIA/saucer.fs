#version 330

uniform sampler2DArray mgTextureUnit0;

in vec4 vFragColor;
smooth in vec3 vTex0;
smooth in vec3 vTex1;
in float blend;

out vec4 outFragColor;

void main(void) 
{ 
  vec4 tex0 = texture(mgTextureUnit0, vTex0);
  vec4 tex1 = texture(mgTextureUnit0, vTex1);
  outFragColor = vFragColor * mix(tex0, tex1, blend);
}
