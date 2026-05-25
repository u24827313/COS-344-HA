#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColour;
uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform vec3 viewPos;

out vec4 FragColor;

void main() {
    float ambientStrength = 0.8;
    vec3 ambient = vec3(ambientStrength);

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-lightDirection);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColour;

    vec3 result = (ambient + diffuse) * objectColour;

    float luminance = dot(result, vec3(0.2126, 0.7152, 0.0722));
    luminance = pow(luminance, 0.85);
    FragColor = vec4(luminance * 0.3, luminance * 1.0, luminance * 0.3, 0.5);
    result = vec3(luminance * 0.1, luminance * 1.4, luminance * 0.1);

}