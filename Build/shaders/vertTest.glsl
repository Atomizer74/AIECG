#version 410


layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec4 normal;

out vec4 vColour;
out vec2 vTexcoord;
out vec4 vNormal;

uniform mat4 projectionViewWorldMatrix;

void main()
{
  vTexcoord = texcoord;
  vColour = colour;
  vNormal = normal;
  gl_Position = projectionViewWorldMatrix * position;
}
