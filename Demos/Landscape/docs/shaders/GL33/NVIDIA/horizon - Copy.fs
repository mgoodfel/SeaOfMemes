#version 330

uniform vec4 fogColor;
smooth in float fog;

out vec4 outFragColor;

void main(void) 
{ 
  outFragColor.rgb = fogColor.rgb;
  outFragColor.a = fog;
}
