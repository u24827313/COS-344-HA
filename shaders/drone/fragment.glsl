#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColour;
uniform vec3 lightDirection;   
uniform vec3 lightColour;
uniform vec3 viewPos;          



out vec4 FragColor;

uniform int isNightVision; 

void main() {
    float ambientStrength = (isNightVision == 1) ? 0.8 : 0.2;

    // Neutral ambient in NV mode so sun colour doesn't bleed into the green tint
    vec3 ambient = (isNightVision == 1)
        ? vec3(ambientStrength)
        : ambientStrength * lightColour;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-lightDirection);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColour;

    vec3 result = (ambient + diffuse) * objectColour;

    if (isNightVision == 1) {
        float luminance = dot(result, vec3(0.2126, 0.7152, 0.0722));
        luminance = pow(luminance, 0.7);
        result = vec3(luminance * 0.1, luminance * 1.4, luminance * 0.1);
    }

    FragColor = vec4(result, 1.0);
}