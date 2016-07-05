#version 410


layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec4 normal;
layout(location = 3) in vec2 texcoord;
layout(location = 4) in vec4 tangent;

out vec4 vColour;
out vec4 vNormal;
out vec2 vTexcoord;
out vec4 vPosition;

uniform mat4 projectionViewWorldMatrix;
uniform mat4 worldMatrix;
uniform float time;

void main()
{
  vColour = colour;
  vNormal = normal;
  vTexcoord = texcoord;
  vPosition = worldMatrix * position; // puts in to world space
  gl_Position = projectionViewWorldMatrix * position;
}
