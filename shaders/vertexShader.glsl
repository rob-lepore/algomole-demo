#version 400

layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec4 aColor; // the color variable has attribute position 1
layout (location = 2 ) in vec3 vertexNormal; // Attributo Normale 2

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 viewPos;

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


out vec4 ourColor; 
out vec3 N,V,L,R;
out vec3 normalInterp;


void main(void)
{
    gl_Position = Projection*View*Model*vec4(aPos, 1.0);
    //vec4 eyePosition= View*Model*vec4(aPos,1.0);

    mat4 G= transpose(inverse(View*Model));
    normalInterp =vec3( G * vec4(vertexNormal,0.0) );


    ourColor = aColor;
    gl_PointSize = 4;
}
