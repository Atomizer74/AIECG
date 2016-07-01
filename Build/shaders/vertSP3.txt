#version 410


layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour;

out vec4 vColour;

uniform mat4 projectionViewWorldMatrix;

uniform float time;
uniform float heightScale;
uniform float freqScale;

void main()
{
  vColour = colour;
  vec4 P = position;
  P.y += sin( (time + position.x) * freqScale ) * heightScale;
  gl_Position = projectionViewWorldMatrix * P;
}
