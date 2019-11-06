#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;  
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
vec3 lightColor;
uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform float shininess;

uniform vec3 delta;

void main()
{
	lightColor = vec3(1.0);
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * vec3(texture(diffuseTex, TexCoords).xyz);
    FragColor = vec4(result + delta, 1.0);
} 