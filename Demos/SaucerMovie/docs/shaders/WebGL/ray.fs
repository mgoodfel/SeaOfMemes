precision mediump float;

uniform sampler2D mgTextureUnit0;
uniform vec3 textureOffset;

varying vec4 vFragColor;
varying vec2 vTex;

void main(void) 
{ 
  vec2 tc;
  tc.xy = vTex.xy + textureOffset.xy;
  gl_FragColor = vFragColor * texture2D(mgTextureUnit0, tc);
}
