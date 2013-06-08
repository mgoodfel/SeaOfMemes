precision mediump float;

uniform mat4 mgModelMatrix;
uniform mat4 mgMVPMatrix;
uniform vec3 mgEyePt;
uniform vec3 mgLightColor;
uniform vec3 mgLightAmbient;
uniform vec4 mgMatColor;
uniform vec2 mgAtlasSize;
uniform vec2 mgTextureSize;

uniform vec3 torchColor;

uniform float fogTopHeight;
uniform float fogTopInten;
uniform float fogBotHeight;
uniform float fogBotInten;
uniform float fogMaxDist;

attribute vec3 vertPoint;
attribute vec3 vertTexCoord;
attribute vec3 vertInten;

varying vec4 vFragColor;
varying vec4 vTex;
varying vec4 vTexOrigin;
varying vec4 vTexSize;
varying float fog;

void main(void) 
{
  vec4 pt;
  pt.xyz = vertPoint.xyz;
  pt.w = 1.0;

  vTex.xy = vertTexCoord.xy;
  float tz = floor(vertTexCoord.z);

  vTexSize.x = mgTextureSize.x/mgAtlasSize.x;
  vTexSize.y = mgTextureSize.y/mgAtlasSize.y;

  float cols = floor(mgAtlasSize.x / mgTextureSize.x);
  vTexOrigin.x = mod(tz, cols) * mgTextureSize.x;
  vTexOrigin.y = floor(tz/cols) * mgTextureSize.y;
  vTexOrigin.x /= mgAtlasSize.x;
  vTexOrigin.y /= mgAtlasSize.y;

  if (vertInten.z < 0.0)
  {
    // handle lit faces
    vFragColor.rgb = mgMatColor.rgb * -vertInten.z;
  }
  else
  {
    // combine ambient lighting, sky light, block light
    vec3 light = mgLightColor * vertInten.y + torchColor * vertInten.z;
    light = min(light, 1.0);
    light = (vertInten.x * light + mgLightAmbient);
    vFragColor.rgb = mgMatColor.rgb * light;
  }
  vFragColor.a = mgMatColor.a;

  // figure vertex coordinates
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
  fog = fog*(endInten+startInten)/2.0;
}
