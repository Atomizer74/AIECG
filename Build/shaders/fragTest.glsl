#version 410

in vec4 vColour;
in vec2 vTexcoord;
in vec4 vNormal;

uniform sampler2D diffuse;
uniform sampler2D normal;

uniform vec3 lightDirection;

out vec4 fragColour;

void main()
{
  vec3 N = normalize(vNormal.xyz);
  vec3 L = normalize(lightDirection);
  float NdL = max( 0, dot(N, L));
  fragColour = texture(diffuse, vTexcoord);
  fragColour.rgb *= NdL;
}
