#version 120 

uniform sampler2D mgTextureUnit0;

varying vec4 vFragColor;
varying vec4 vTex;
varying vec4 vTexOrigin;
varying vec4 vTexSize;

void main(void) 
{ 
  vec2 tc;
  tc.x = vTexOrigin.x + vTexSize.x * mod(vTex.x, 1.0);
  tc.y = vTexOrigin.y + vTexSize.y * mod(vTex.y, 1.0);

  gl_FragColor = vFragColor * texture2D(mgTextureUnit0, tc);
}
