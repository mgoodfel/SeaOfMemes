precision mediump float;

uniform vec4 fogColor;
uniform float fogTopHeight;
uniform float fogTopInten;
uniform float fogBotHeight;
uniform float fogBotInten;
uniform float fogMaxDist;

varying vec3 modelPt;
varying float fogSlope;

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
  fog = fog  * (startInten+endInten)/2.0;

  gl_FragColor.rgb = fogColor.rgb;
  gl_FragColor.a = fog;
}
