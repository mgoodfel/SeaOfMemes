#version 120

uniform sampler2D textureUnit0;

varying vec2 vTextureOrigin;
varying vec2 vTextureSize;
varying vec4 vFragColor;
varying vec2 vTex;

void main(void) 
{
  vec2 vClampTex = mod(vTex, 1.0);
  vec2 vAtlasTex = vTextureOrigin + vTextureSize * vClampTex;

  gl_FragColor = vFragColor * texture2D(textureUnit0, vAtlasTex);
}
