#version 120

uniform sampler2D mgTextureUnit0;

varying vec4 vFragColor;
varying vec4 vTexSize;
varying vec4 vTex0;
varying vec4 vTexOrigin0;
varying vec4 vTex1;
varying vec4 vTexOrigin1;
varying float blend;

void main(void) 
{ 
  vec2 tc;
  tc.x = vTexOrigin0.x + vTexSize.x * mod(vTex0.x, 1.0);
  tc.y = vTexOrigin0.y + vTexSize.y * mod(vTex0.y, 1.0);

  vec4 tex0 = texture2D(mgTextureUnit0, tc);

  tc.x = vTexOrigin1.x + vTexSize.x * mod(vTex1.x, 1.0);
  tc.y = vTexOrigin1.y + vTexSize.y * mod(vTex1.y, 1.0);
  vec4 tex1 = texture2D(mgTextureUnit0, tc);

  gl_FragColor = vFragColor * mix(tex0, tex1, blend);
}
