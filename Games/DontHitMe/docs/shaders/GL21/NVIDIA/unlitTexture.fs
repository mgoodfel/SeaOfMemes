#version 120

uniform sampler2D mgTextureUnit0;
uniform vec3 textureOrigin;

varying vec4 vFragColor;
varying vec2 vTex;

void main(void) 
{ 
  vec2 tc;
  tc.x = vTex.x + textureOrigin.x;
  tc.y = vTex.y + textureOrigin.y;

  gl_FragColor = vFragColor * texture2D(mgTextureUnit0, tc);
}
