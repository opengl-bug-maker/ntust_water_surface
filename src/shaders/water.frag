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
uniform vec3 lightPos;
uniform sampler2D u_texture;

void main()
{   
	float ambientStrength = 0.99f;
	vec3 ambient = ambientStrength * light_color;

	vec3 norm = normalize(f_in.normal);
	vec3 lightDir = normalize(lightPos - f_in.position);  

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light_color;

	vec3 result = (ambient + diffuse) * vec3(0.92f, 0.5f, 0.77f);
	f_color = vec4(result, 1.0);
    //vec3 color = vec3(texture(u_texture, f_in.texture_coordinate))*f_in.color;
    //f_color = vec4(0.92f, 0.5f, 0.77f, 1.0f);
}