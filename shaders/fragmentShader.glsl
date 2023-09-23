#version 400

in vec4 ourColor;
//in vec3 N,V,L,R;
in vec3 normalInterp;

out vec4 FragColor;

struct Light{
    vec4 vector;
	vec3 color;
	float power;
 };
uniform Light light;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;


void main(void)
{

    vec3 N= normalize(normalInterp);     
    //vec3 V= normalize(viewPos - eyePosition.xyz);
    
    vec3 L = normalize(light.vector.xyz);
    //vec3 R = reflect(-L,N); 

    float strength = 0.1;
    
    vec3 lightCont = light.power / 10 * light.color;
    
    vec3 ambient = strength * lightCont * material.ambient * vec3(ourColor) + vec3(0.5);

    float diff = max(dot(N, L),0.0);
    vec3 diffuse = diff * lightCont * material.diffuse * vec3(ourColor);

    //float spec = pow(max(dot(V, R), 0.0), material.shininess);
    //vec3 specular = strength * spec * lightCont;

    vec3 res = (ambient + diffuse) * material.diffuse;    
    
    FragColor = vec4(res,1);
}
