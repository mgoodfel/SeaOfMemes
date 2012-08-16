#version 330

uniform mat4 mgProjMatrix;
uniform vec2 mgOrigin;

in vec2 vertPoint;
in vec2 vertTexCoord0;

smooth out vec2 vTex;

void main(void) 
{ 
  vTex = vertTexCoord0;

  vec4 position;
  position.xy = vertPoint + mgOrigin;
  position.z = 0.0;
  position.w = 1.0;

  gl_Position = mgProjMatrix * position; 
}
