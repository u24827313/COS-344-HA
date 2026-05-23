#version 330 core
in  vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;

void main()
{

    FragColor.x = (texture(skybox, TexCoords).x + texture(skybox, TexCoords).y + texture(skybox, TexCoords).z) / 3;

    FragColor.y = (texture(skybox, TexCoords).x + texture(skybox, TexCoords).y + texture(skybox, TexCoords).z) / 3;

    FragColor.z = (texture(skybox, TexCoords).x + texture(skybox, TexCoords).y + texture(skybox, TexCoords).z) / 3;

    FragColor.w = 1.0;
}