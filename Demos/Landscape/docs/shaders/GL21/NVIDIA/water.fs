#version 120

uniform sampler2D mgTextureUnit0;
uniform vec4 fogColor;

varying vec4 vFragColor;
varying vec2 vTex;
varying float fog;

void main(void) 
{ 
  gl_FragColor = mix(vFragColor * texture2D(mgTextureUnit0, vTex), fogColor, fog);
}
