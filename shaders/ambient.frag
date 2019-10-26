#version 400 core

uniform vec3 terrain_color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 view_pos;

uniform float ambient_strenght = 0.2;
uniform float specular_strenght = 0.2;

in vec3 o_normal;
in vec3 o_pos;

out vec4 out_color;

void main(void)
{
    // ambient
    vec3 ambient = ambient_strenght * light_color;
    // diffuse
    vec3 norm = normalize(o_normal);
    vec3 light_dir = normalize(light_pos - o_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color * 0.8;
    // specular
    vec3 view_dir = normalize(view_pos - o_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    // 32 = 2 pow that dictates how specular the object will be
    // goes up to 256
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 16);
    vec3 specular = specular_strenght * spec * light_color;
    
    vec3 result = (ambient + diffuse + specular) * terrain_color;
    out_color = vec4(result, 1.0);
}
