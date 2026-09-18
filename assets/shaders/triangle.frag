#version 460 core

in vec3 vertexColor;
out vec4 FragColor;

uniform vec3 uColor;

void main()
{
    FragColor = vec4(vertexColor * uColor, 1.0);}