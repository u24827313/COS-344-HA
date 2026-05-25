#version 330 core
in  vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;

void main()
{
    vec3 colour = texture(skybox, TexCoords).rgb;
    float luminance = dot(colour, vec3(0.2126, 0.7152, 0.0722));
    luminance = pow(luminance, 0.85);
    FragColor = vec4(luminance * 0.3, luminance * 1.0, luminance * 0.3, 0.5);
}