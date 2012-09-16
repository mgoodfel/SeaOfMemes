#version 120

uniform sampler2D mgTextureUnit0;

varying vec4 vFragColor;
varying vec2 vTex;
        
void main(void) 
{ 
  gl_FragColor = vFragColor * texture2D(mgTextureUnit0, vTex);
}
