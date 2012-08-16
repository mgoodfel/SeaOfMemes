#version 330

uniform mat4 mgMVPMatrix;
uniform mat3 mgNormalMatrix;
uniform vec3 mgLightDir;
uniform vec3 mgLightColor;
uniform vec3 mgLightAmbient;
uniform vec4 mgMatColor;

uniform float textureCycle;

in vec2 vertTexCoord0;
in vec4 vertPoint;
in vec3 vertNormal;

out vec4 vFragColor;
smooth out vec3 vTex0;
smooth out vec3 vTex1;
out float blend;

void main(void) 
{ 
  vec3 eyeNormal = mgNormalMatrix * vertNormal;
  float lightInten = max(0.0, dot(eyeNormal, mgLightDir));

  vFragColor.rgb = mgMatColor.rgb * (mgLightColor * lightInten + mgLightAmbient);
  vFragColor.a = mgMatColor.a;
  vTex0.xy = vertTexCoord0;
  vTex1.xy = vertTexCoord0;

  if (textureCycle >= 0)
  {
    // blend between two animation textures
    vTex0.z = floor(textureCycle);
    blend = textureCycle - vTex0.z;
    vTex1.z = mod(1+vTex0.z, 10);
  }
  else
  {
    // turn animation off
    blend = 0.0;
    vTex0.z = 10;
    vTex1.z = 10;
  }

  gl_Position = mgMVPMatrix * vertPoint; 
}
