#version 330

uniform vec4 fogColor;
uniform float fogTopHeight;
uniform float fogTopInten;
uniform float fogBotHeight;
uniform float fogBotInten;
uniform float fogMaxDist;

smooth in vec3 modelPt;
flat in float fogSlope;

out vec4 outFragColor;

void main(void) 
{ 
  float dy = modelPt.y;
  if (abs(dy) < 0.01)
    dy = 0.01;  // give it small slope to get intercepts

  float startY = clamp(0.0, fogBotHeight, fogTopHeight);
  float endY = clamp(dy, fogBotHeight, fogTopHeight);

  float startInten = fogBotInten + (startY - fogBotHeight) * fogSlope;
  float endInten = fogBotInten + (endY - fogBotHeight) * fogSlope;

  float startDist = startY / dy;
  float endDist = endY / dy;

  float fog = (endDist - startDist)/fogMaxDist;
  fog = clamp(fog, 0.0, 1.0);
  fog = fog  * (startInten+endInten)/2;

  outFragColor.rgb = fogColor.rgb;
  outFragColor.a = fog;
}
