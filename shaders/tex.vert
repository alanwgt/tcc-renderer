#version 400 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texture_coords;

out vec2 out_texture_coords;

uniform mat4 transformation_matrix;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;

void main(void)
{
    gl_Position = projection_matrix * view_matrix * transformation_matrix * vec4(pos, 1.0);
    out_texture_coords = texture_coords;
}
