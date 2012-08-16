#version 330

uniform sampler2D mgTextureUnit0;
uniform vec3 textureOffset;

in vec4 vFragColor;
smooth in vec2 vTex;

out vec4 outFragColor;

void main(void) 
{ 
  vec2 tc;
  tc.xy = vTex.xy + textureOffset.xy;
  outFragColor = vFragColor * texture(mgTextureUnit0, tc);
}
