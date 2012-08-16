#version 330 

uniform mat4 mgModelMatrix;
uniform mat4 mgMVPMatrix;
uniform vec3 mgEyePt;
uniform vec3 mgLightColor;
uniform vec3 mgLightAmbient;
uniform vec4 mgMatColor;

uniform vec3 torchColor;

uniform float fogTopHeight;
uniform float fogTopInten;
uniform float fogBotHeight;
uniform float fogBotInten;
uniform float fogMaxDist;

in ivec3 vertPoint;

smooth out vec4 vFragColor;
smooth out vec3 vTex;
smooth out float fog;

void main(void) 
{
  vec4 pt;
  pt.x = float(vertPoint.x & 0x7FFF)/512.0;
  pt.y = float((vertPoint.x >> 15) & 0x7FFF)/512.0;
  pt.z = float(vertPoint.y & 0x7FFF)/512.0;
  pt.w = 1.0;

  // unpack texture
  vTex.x = float(vertPoint.z & 0x7FF)/128.0;
  vTex.y = float((vertPoint.z >> 11) & 0x7FF)/128.0;
  vTex.z = float((vertPoint.z >> 22) & 0x3FF);

  // unpack light intensities
  float ambientInten = float((vertPoint.y >> 15) & 0xF)/15.0;
  float skyInten = pow(0.8, (vertPoint.y >> 19) & 0xF);
  float blockInten = pow(0.8, (vertPoint.y >> 23) & 0xF);
  int lampFlag = (vertPoint.y >> 27) & 0x1;

  if (lampFlag != 0)
  {
    // handle lit faces
    vFragColor.rgb = mgMatColor.rgb * blockInten;
  }
  else
  {
    // combine ambient lighting, sky light, block light
    vec3 light = mgLightColor * skyInten + torchColor * blockInten;
    light = min(light, 1.0);
    light = (ambientInten * light + mgLightAmbient);
    vFragColor.rgb = mgMatColor.rgb * light;
  }
  vFragColor.a = mgMatColor.a;

  // figure Pt coordinates
  gl_Position = mgMVPMatrix * pt; 

  // figure world-space coordinates of point
  vec4 posn = mgModelMatrix * pt;

  float startY = clamp(mgEyePt.y, fogBotHeight, fogTopHeight);
  float endY = clamp(posn.y, fogBotHeight, fogTopHeight);

  float fogSlope = (fogTopInten - fogBotInten) / (fogTopHeight - fogBotHeight);

  float startInten = fogBotInten + (startY - fogBotHeight) * fogSlope;
  float endInten = fogBotInten + (endY - fogBotHeight) * fogSlope;

  // figure distance through fog at start and end points
  float startDist, endDist;
  posn.xyz -= mgEyePt;
  float dy = posn.y / length(posn); 

  // if ray is horizontal
  if (abs(dy) < 0.001)
  {
    // entire ray is within fog
    startDist = 0.0;
    endDist = length(posn);
  }
  else
  {
    // figure distance to entrance of fog, exit of fog
    startDist = (startY - mgEyePt.y) / dy;
    endDist = (endY - mgEyePt.y) / dy;
  }

  fog = (abs(endDist - startDist))/fogMaxDist;
  fog = clamp(fog, 0.0, 1.0);
  fog = fog*(endInten+startInten)/2;
}
