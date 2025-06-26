#version 430 core
in float fragcolor;
in vec2 textureCoords;
in vec3 Normal;
in vec3 fragPos;

out vec4 frag;

//uniform vec3 lightPos;
vec3 lightPos = vec3(-1,2,-7);
uniform sampler2D texslot;

vec3 lightColor = vec3(1,1,1);
float ambientStrength = 0.1;


vec3 calculateLight()
{
    vec3 ambient = ambientStrength * lightColor;
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = diff*lightColor;
	return vec3((ambient+diffuse) * 1);
};

void main ()
{
	vec4 texColor = texture(texslot,textureCoords);
	frag = texColor * vec4(calculateLight(),1); 
}