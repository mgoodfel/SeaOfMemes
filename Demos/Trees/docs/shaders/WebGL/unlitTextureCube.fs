precision mediump float;

uniform samplerCube mgTextureUnit0;

varying vec4 vFragColor;
varying vec3 vTex;
       
void main(void) 
{ 
  gl_FragColor = vFragColor * textureCube(mgTextureUnit0, vTex);
}
