#version 400 core

layout (location = 0) in vec3 pos;
out vec3 color;

uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat4 transformation_matrix;

void main(void)
{
    gl_Position = projection_matrix * view_matrix * transformation_matrix * vec4(pos, 1.0);
    /* color = vec3(pos.x + 0.5, 1.0, pos.y + 0.5); */
    color = vec3(1.0);
}
