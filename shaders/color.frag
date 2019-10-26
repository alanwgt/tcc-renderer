#version 330 core

out vec4 FragColor;
in vec3 mColor;

uniform float time;

void main()
{
    float t = sin(time);
    FragColor = vec4(mColor.r * sin(t), mColor.g * cos(time), mColor.b * t, 1.0);
}
