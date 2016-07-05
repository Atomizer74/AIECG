#version 410

in vec4 vColour;
in vec4 vNormal;
in vec2 vTexcoord;
in vec4 vPosition;

uniform vec4 cameraPosition;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform float time;

// Material properties
uniform vec3 Kd; // diffuse colour
uniform vec3 Ks; // specular colour
uniform float specularPower; // specular power

// Light properties
struct Light {
  vec3 position;
  vec3 direction;
  vec3 colour;
};

uniform Light light;
uniform vec3 ambientLight;


out vec4 fragColour;

void main()
{
  vec3 N = normalize(vNormal.xyz);
  vec3 L = normalize(light.position - vPosition.xyz);
  vec3 E = normalize(cameraPosition.xyz - vPosition.xyz);
  vec3 R = reflect(-L, N);

  // Specular term
  float RdE = max( 0, dot(R, E));

  // Lambert term
  float NdL = max( 0, dot(N, L));

  // ambientLight
  vec3 ambient = Kd * ambientLight;

  // diffuse
  vec3 diffuse = Kd * light.colour * NdL;

  // specular
  vec3 specular = Ks * light.colour * pow(RdE, specularPower);

  //fragColour = texture(diffuseTexture, vTexcoord);
  fragColour = vec4(ambient + diffuse + specular, 1);
}
