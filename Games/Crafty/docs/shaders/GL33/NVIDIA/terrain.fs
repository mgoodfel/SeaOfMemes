#version 330 

uniform sampler2D mgTextureUnit0;
uniform vec4 fogColor;

smooth in vec4 vFragColor;
smooth in vec2 vTex;
smooth in float fog;

out vec4 outFragColor;

void main(void) 
{ 
  outFragColor = mix(vFragColor * texture(mgTextureUnit0, vTex), fogColor, fog);
}
