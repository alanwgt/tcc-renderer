#version 400 core

in vec2 out_texture_coords;
out vec4 out_color;

uniform sampler2D texture_sampler;

void main(void)
{
    out_color = texture(texture_sampler, out_texture_coords);
    /* out_color = vec4(out_texture_coords.y, out_texture_coords.x, 0.5, 1.0); */
}
