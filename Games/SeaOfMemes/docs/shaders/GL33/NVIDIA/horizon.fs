#version 330 

uniform vec3 mgEyePt;
uniform sampler2D mgTextureUnit0;

uniform vec4 fogColor;
uniform float fogTopHeight;
uniform float fogTopInten;
uniform float fogBotHeight;
uniform float fogBotInten;
uniform float fogMaxDist;

smooth in vec4 vFragColor;
smooth in vec2 vTex;
smooth in vec3 modelPt;
flat in float fogSlope;

out vec4 outFragColor;

void main(void) 
{ 
  vec3 posn = modelPt;

  float startY = clamp(mgEyePt.y, fogBotHeight, fogTopHeight);
  float endY = clamp(posn.y, fogBotHeight, fogTopHeight);

  float startInten = fogBotInten + (startY - fogBotHeight) * fogSlope;
  float endInten = fogBotInten + (endY - fogBotHeight) * fogSlope;

  posn -= mgEyePt;
  float dy = posn.y / length(posn); 
  if (dy == 0.0)
    dy = 0.00001;  // give it small slope to get intercepts

  float startDist = (startY - mgEyePt.y) / dy;
  float endDist = (endY - mgEyePt.y) / dy;

  float fog = (abs(endDist - startDist))/fogMaxDist;
  fog = clamp(fog, 0.0, 1.0);
  fog = fog*(endInten+startInten)/2;

  outFragColor = mix(vFragColor * texture(mgTextureUnit0, vTex), fogColor, fog);
}
