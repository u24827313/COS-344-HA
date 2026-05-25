#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColour;
uniform vec3 lightDirection;   
uniform vec3 lightColour;
uniform vec3 viewPos;          

out vec4 FragColor;

void main() {
    
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColour;

    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-lightDirection);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColour;

    vec3 result = (ambient + diffuse) * objectColour;
    FragColor = vec4(result, 1.0);
}