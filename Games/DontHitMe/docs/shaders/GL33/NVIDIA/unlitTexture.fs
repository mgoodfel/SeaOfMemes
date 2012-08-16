#version 330

uniform sampler2D mgTextureUnit0;
uniform vec3 textureOrigin;

in vec4 vFragColor;
smooth in vec2 vTex;

out vec4 outFragColor;

void main(void) 
{ 
  vec2 tc;
  tc.x = vTex.x + textureOrigin.x;
  tc.y = vTex.y + textureOrigin.y;

  outFragColor = vFragColor * texture(mgTextureUnit0, tc);
}
