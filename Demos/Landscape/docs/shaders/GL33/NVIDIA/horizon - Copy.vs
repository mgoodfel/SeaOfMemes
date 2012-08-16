#version 330

uniform mat4 mgMVPMatrix;

uniform float fogTopHeight;
uniform float fogTopInten;
uniform float fogBotHeight;
uniform float fogBotInten;
uniform float fogMaxDist;

in vec3 vertPoint;

smooth out float fog;

void main(void) 
{ 
  vec4 pt;
  pt.xyz = vertPoint;
  pt.w = 1.0;
  gl_Position = mgMVPMatrix * pt; 

  if (pt.y == 0.0)
    pt.y = 0.00001;  // give it small slope to get intercepts

  float startY = clamp(0.0, fogBotHeight, fogTopHeight);
  float endY = clamp(pt.y, fogBotHeight, fogTopHeight);

  float fogSlope = (fogTopInten - fogBotInten) / (fogTopHeight - fogBotHeight);

  float startInten = fogBotInten + (startY - fogBotHeight) * fogSlope;
  float endInten = fogBotInten + (endY - fogBotHeight) * fogSlope;

  float startDist = startY / pt.y;
  float endDist = endY / pt.y;

  fog = (endDist - startDist)/fogMaxDist;
  fog = clamp(fog, 0.0, 1.0);
  fog = fog  * (startInten+endInten)/2;

/*
  float dy = vertPoint.y / length(vertPoint);

  // figure fog plane intercepts
  float topDist = 100000.0;  // infinity
  float botDist = -100000.0;  // infinity
  if (dy != 0.0)
  {
    topDist = fogTopHeight / dy;
    botDist = fogBotHeight / dy;
  }
  float startDist = min(botDist, topDist);
  startDist = clamp(startDist, 0.0, 1.0);

  float endDist = max(botDist, topDist);
  endDist = clamp(endDist, 0.0, 1.0);

  // figure fog intensity
  float fogHeight = (pt.y + dy * startDist - fogBotHeight)/(fogTopHeight-fogBotHeight);
  float startInten = fogBotInten + fogHeight * (fogTopInten-fogBotInten);

  fogHeight = (pt.y + dy * endDist - fogBotHeight)/(fogTopHeight-fogBotHeight);
  float endInten = fogBotInten + fogHeight * (fogTopInten-fogBotInten);

  fog = (endDist - startDist)/fogMaxDist;
  fog = clamp(fog, 0.0, 1.0);
  fog = fog  * (startInten+endInten)/2;
*/
}
