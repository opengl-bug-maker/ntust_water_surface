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


vec2 intersectCube(vec3 origin, vec3 ray, vec3 cubeMin, vec3 cubeMax) {
    vec3 tMin = (cubeMin - origin) / ray;
    vec3 tMax = (cubeMax - origin) / ray;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);
    return vec2(tNear, tFar);
}
vec3 getWallColor(vec3 point) {
    float scale = 0.5;

    vec3 wallColor;
    //vec3 normal;
    if (abs(point.x) > 99.999) {
        wallColor = texture2D(u_texture, point.yz * 0.005 + vec2(1.0, 0.5)).rgb;
        //normal = vec3(-point.x, 0.0, 0.0);
    } else if (abs(point.z) > 99.999) {
        wallColor = texture2D(u_texture, point.yx * 0.005 + vec2(1.0, 0.5)).rgb;
        //normal = vec3(0.0, 0.0, -point.z);
    } else {
        wallColor = texture2D(u_texture, point.xz * 0.005 + 0.5).rgb;
        //normal = vec3(0.0, 1.0, 0.0);
    }

    //scale /= length(point);

    return wallColor * scale;
}

vec3 getWallTexture(vec3 origin, vec3 ray) {
 //wall 1x1x1 cube
 //origin (0~1, 0~1, 0~1)
 //ray been normalize

    vec3 color;

    vec2 t = intersectCube(origin, ray, vec3(-100.0, -0.0, -100.0), vec3(100.0, 2.0, 100.0));

    if (ray.y < 0.0) {
        color = getWallColor(origin + ray * t.y);
        color *= light_color;
    } else {
        vec3 hit = origin + ray * t.y;
        if (hit.y < 2.0 / 12.0) {
            color = getWallColor(origin + ray * t.y);
        } else {
            color = texture(skybox, ray).rgb;
            //color += vec3(pow(max(0.0, dot(light, ray)), 5000.0)) * vec3(10.0, 8.0, 6.0);
        }
    }
    
    //color *= light_color;
    return color;
}

void main()
{   
	float ambientStrength = 1.0f;
	vec3 ambient = ambientStrength * light_color;
	
	vec3 norm = normalize(f_in.normal);
    //fake world position
	vec3 lightDir = normalize(lightPos - f_in.position);  // 0, 5, 0 - x, 5 ,y
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light_color;
	
	vec3 half_vector = normalize(normalize(lightPos) + normalize(eye_position));
	
	vec3 specular = pow(max(0.0, dot(norm, half_vector)), 77.0f) * vec3(1.0, 1.0, 1.0);

	vec3 result = (ambient + diffuse) * vec3(0.3f, 0.5f, 0.92f) + specular;
	//result = result * vec3(texture(u_texture, f_in.texture_coordinate));

    //fake world position too
    vec3 incomingRay = f_in.position - eye_position;
    incomingRay = normalize(incomingRay);

	float fresnel = mix(0.25, 1.0, pow(1.0 - dot(f_in.normal, -incomingRay), 3.0));


	vec3 I = normalize(f_in.position-eye_position);
    vec3 R = reflect(I,normalize(norm));
	vec4 reflected = vec4(texture(skybox, R).rgb, 1.0);
	
	vec3 Refract = refract(I, normalize(norm), 1.00 / 1.33);
    vec4 refracted = vec4(getWallTexture(f_in.position, Refract), 1.0);

    //vec4 fl_fr = mix(refracted, reflected, fresnel);
    vec4 fl_fr = mix(refracted, reflected, 0.7);


    //vec3 color = vec3(texture(u_texture, f_in.texture_coordinate))*f_in.color;
    //f_color = vec4(0.92f, 0.5f, 0.77f, 1.0f);
	//f_color =  reflected*0.7+vec4(result, 1.0f)*0.3f;
	f_color =  fl_fr*0.8+vec4(result, 1.0f)*0.2f;
	//f_color =  reflected;
	//f_color =  refracted;
}