#version 410

in vec4 vColour;
in vec2 vTexcoord;
in vec4 vNormal;
in float height;

uniform sampler2D diffuse;
uniform sampler2D dirt;
uniform vec2 perlinHeights;

out vec4 fragColour;

void main()
{
	vec3 rgb = texture(diffuse, vTexcoord).rgb;
	vec3 rgbDirt = texture(dirt, vTexcoord).rgb;
	float h = 1 - ((perlinHeights.y - height) / (perlinHeights.y - perlinHeights.x));

	fragColour = vec4(mix(rgbDirt, rgb, h), 1);
}
