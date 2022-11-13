#version 430 core
out vec4 f_color;

in V_OUT
{
   vec3 position;
   vec3 normal;
   vec2 texture_coordinate;
   vec3 color;
} f_in;

uniform vec3 u_color;
uniform vec3 light_color;

uniform sampler2D u_texture;

void main()
{   
    vec3 color = vec3(texture(u_texture, f_in.texture_coordinate))*f_in.color;
    //f_color = vec4(color, 1.0f);
    f_color = vec4(0.32f, 0.5f, 0.77f, 1.0f);
}