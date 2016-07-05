#version 410


layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec4 normal;
layout(location = 3) in vec4 tangent;
layout(location = 4) in float roughness;

out vec4 vPosition;
out vec2 vTexcoord;
out vec4 vNormal;
out vec4 vTangent;
out vec4 vBiTangent;
out float vRoughness;

uniform mat4 projectionViewWorldMatrix;
uniform mat4 worldMatrix;
uniform float time;

void main()
{
	vPosition = worldMatrix * position; // puts in to world space
	vTexcoord = texcoord;
	vNormal = normal;
	vTangent = vec4(tangent.xyz, 0);
	vBiTangent = vec4(cross(normal.xyz, tangent.xyz) * tangent.w, 0);
	vRoughness = roughness;
	gl_Position = projectionViewWorldMatrix * position;
}
