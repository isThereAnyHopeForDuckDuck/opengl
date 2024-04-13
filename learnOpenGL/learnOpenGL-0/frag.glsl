#version 330 core

out vec4 FragColor;
in vec4 vertexColor;

uniform float greenValue;

void main()
{
   FragColor = vec4(vertexColor.r, greenValue, vertexColor.b, vertexColor.a);
}