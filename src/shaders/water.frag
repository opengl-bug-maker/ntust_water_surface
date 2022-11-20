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
uniform vec3 eye_position;
uniform sampler2D u_texture;
uniform samplerCube skybox;
void main()
{   
	float ambientStrength = 1.0f;
	vec3 ambient = ambientStrength * light_color;
	
	vec3 norm = normalize(f_in.normal);
	vec3 lightDir = normalize(lightPos - f_in.position);  // 0, 5, 0 - x, 5 ,y
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light_color;
	
	vec3 half_vector = normalize(normalize(lightPos) + normalize(eye_position));
	
	vec3 specular = pow(max(0.0, dot(norm, half_vector)), 77.0f) * vec3(1.0, 1.0, 1.0);

	vec3 result = (ambient + diffuse) * vec3(0.3f, 0.5f, 0.92f) + specular;
	//result = result * vec3(texture(u_texture, f_in.texture_coordinate));


	vec3 I = normalize(f_in.position-eye_position);
    vec3 R = reflect(I,normalize(norm));
	vec4 reflected = vec4(texture(skybox, R).rgb, 1.0);
	

    //vec3 color = vec3(texture(u_texture, f_in.texture_coordinate))*f_in.color;
    //f_color = vec4(0.92f, 0.5f, 0.77f, 1.0f);
	f_color = reflected;
}