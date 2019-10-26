#version 400 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat4 transformation_matrix;

out vec3 o_normal;
out vec3 o_pos;

void main(void)
{
    o_pos = vec3(transformation_matrix * vec4(pos, 1.0));
    gl_Position = projection_matrix * view_matrix * vec4(o_pos, 1.0);

    /* o_normal = normal; */
    o_normal = mat3(transpose(inverse(transformation_matrix))) * normal;
}

