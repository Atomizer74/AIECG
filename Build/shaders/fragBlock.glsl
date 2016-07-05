#version 410

// From Vertex Shader
in vec4 vPosition;
in vec2 vTexcoord;
in vec4 vNormal;
in vec4 vTangent;
in vec4 vBiTangent;
in float vRoughness;

// Material properties
uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D specular;
uniform float specularPower;
uniform float fresnelScale;

uniform vec4 cameraPosition;
uniform float time;


// Light properties
struct Light {
  vec3 position;
  vec3 direction;
  vec3 colour;
  float intensity;
};

uniform Light light;
uniform vec3 ambientLight;

// Outputs
out vec4 fragColour;

// Functions
float OrenNayarDiffuse(vec3 N, vec3 L, vec3 E);
float CookTorranceSpecular(vec3 N, vec3 L, vec3 E);

void main()
{
	mat3 TBN = mat3(
		normalize(vTangent.xyz),
		normalize(vBiTangent.xyz),
		normalize(vNormal.xyz));

	vec3 Kd = texture(diffuse, vTexcoord).xyz;
	vec3 Kn = texture(normal, vTexcoord).xyz * 2.0 - 1.0;
	// Invert the Y coordinate of the normal map
	Kn.y = -Kn.y;
	vec3 Ks = texture(specular, vTexcoord).xyz;

	// transform from texture / tangent space to world space
	vec3 N = normalize(TBN * Kn);
	//vec3 L = normalize(light.position - vPosition); // This is for a point light
	vec3 L = normalize(light.direction); // This is for a directional light
	vec3 E = normalize(cameraPosition.xyz - vPosition.xyz);
	vec3 R = reflect(-L, N);
	
	// Specular term
	float RdE = max( 0, dot(R, E));
	
	// Lambert term
	float NdL = max( 0, dot(N, L)); // L is a vector the light is coming from(surface to light)
	float NdE = max( 0, dot(N, E)); // E is a vector from the surface to the viewer
	float OrenNayar = OrenNayarDiffuse(N, L, E);
	
	// ambientLight
	vec3 ambient = Kd * ambientLight;
	
	// diffuse
	//vec3 diffuse = Kd * light.colour * NdL * light.intensity;
	vec3 diffuse = Kd * light.colour * OrenNayar * light.intensity;
	
	// specular
	float CookTorrance = CookTorranceSpecular(N, L, E);
	//vec3 specular = Ks * light.colour * pow(RdE, specularPower) * light.intensity;
	vec3 specular = Ks * light.colour * CookTorrance * light.intensity;
	
	// output final colour
	fragColour = vec4(ambient + diffuse + specular, 1);
}

float OrenNayarDiffuse(vec3 N, vec3 L, vec3 E)
{
	float R2 = vRoughness * vRoughness;

	// N is the surface normal
	float NdL = max( 0, dot(N, L)); // L is a vector the light is coming from(surface to light)
	float NdE = max( 0, dot(N, E)); // E is a vector from the surface to the viewer
	
	// Oren-Nayar Diffuse Term
	float A = 1.0f - 0.5f * R2 / (R2 + 0.33f);
	float B = 0.45f * R2 / (R2 + 0.09f);
	
	// CX = max(0, cos(r,i))
	vec3 lightProjected = normalize( L - N * NdL );
	vec3 viewProjected = normalize( E - N * NdE );
	float CX = max(0.0f, dot(lightProjected, viewProjected));
	
	// DX = sin(alpha) * tan(beta)
	float alpha = sin( max(acos(NdE), acos(NdL)) );
	float beta = tan( min(acos(NdE), acos(NdL)) );
	float DX = alpha * beta;
	
	// Calculate Oren-Nayar, replaces the Phong Lambertian Term
	return NdL * (A + B * CX * DX);
}

float CookTorranceSpecular(vec3 N, vec3 L, vec3 E)
{
	vec3 H = normalize( L + E ); // light and view half vector
	float R2 = vRoughness * vRoughness;
	float HdE = dot(H, E);
	float NdL = dot(N, L);
	float NdE = dot(N, E);
	float NdH = max( 0.0f, dot(N, H) );
	float NdH2 = NdH * NdH;
	float e = 2.71828182845904523536028747135f;
	float pi = 3.1415926535897932384626433832f;

	// Beckmann Distribution Function D
	float exponent = -(1 - NdH2) / (NdH2 * R2);
	float D = pow(e, exponent) / (R2 * NdH2 * NdH2);

	// Fresnel Term F
	float F = mix( pow( 1 - HdE, 5), 1, fresnelScale);

	// Geometric Attenuation Factor
	float X = 2.0f * NdH / dot(E, H);
	float G = min(1, min(X * NdL, X * NdE));

	return max( (D*G*F) / (NdE * pi), 0.0f );
}
