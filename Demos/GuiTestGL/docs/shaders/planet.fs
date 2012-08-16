#version 120 

uniform vec3 mgLightColor;
uniform vec3 mgLightAmbient;
uniform vec4 mgMatColor;
uniform vec3 specularColor;

uniform samplerCube mgTextureUnit0;
uniform samplerCube mgTextureUnit1;

uniform vec3 modelLightDir;
uniform vec3 modelEye;

varying vec3 vFragPt;

void main(void) 
{ 
  // calculate intersection between ray from eye and sphere radius 1.0
  vec3 PmE = vFragPt - modelEye;
  float a = dot(PmE, PmE);
  float b = 2.0*dot(PmE, modelEye);
  float c = dot(modelEye, modelEye) - 1.0;
  float root = b*b - 4.0*a*c;

  // if no intersection, skip pixel
  if (root < 0.0)
    discard;
  root = sqrt(root);
  float t = (-b - root)/(2.0*a);
  vec3 hitPt = modelEye + PmE*t;

  vec3 fragColor;

  // get terrain texture at hit point
  vec4 terrain = textureCube(mgTextureUnit0, hitPt);
  fragColor = terrain.rgb * mgMatColor.rgb;

  // get cloud texture at hit point
  vec4 clouds = textureCube(mgTextureUnit1, hitPt);
  fragColor = mix(fragColor, vec3(0.9), clouds.g);

  // figure diffuse lighting
  float lightInten = max(0.0, dot(hitPt, modelLightDir));
  fragColor *= mgLightColor * lightInten + mgLightAmbient;

  // figure specular intensity (blinn)
  vec3 halfAngle = normalize(modelEye - hitPt);
  halfAngle += modelLightDir;
  halfAngle = normalize(halfAngle);

  float shiny = terrain.a * (1-clouds.g);
  float blinn = dot(hitPt, halfAngle);
  blinn = clamp(blinn, 0.0, 1.0);
  blinn = pow(blinn, 30.0) * shiny;

  // add specular color only if not pointing away from light
  if (lightInten > 0.0)
    fragColor += blinn*specularColor;

  vec3 light = vec3(156/255.0, 37/255.0, 28/255.0);
  fragColor = mix(fragColor, light, (1-clouds.g)*clouds.r);

  // set output
  vec4 outColor;
  outColor.rgb = fragColor;
  outColor.a = mgMatColor.a;
  
  gl_FragColor = outColor;
}
