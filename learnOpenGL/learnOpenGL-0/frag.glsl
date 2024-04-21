#version 330 core

out vec4 FragColor;
in vec4 vertexColor;
in vec2 uv;

uniform float greenValue;
uniform sampler2D texIdx;

void main()
{
   //FragColor = vec4(vertexColor.r, greenValue, vertexColor.b, vertexColor.a);
   //FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, vertexColor.a);
   FragColor = texture(texIdx, uv);
}