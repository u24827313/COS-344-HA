#version 330 core
in  vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;

void main()
{

    FragColor.x = 1.0 - texture(skybox, TexCoords).x;

    FragColor.y = 1.0 - texture(skybox, TexCoords).y;

    FragColor.z = 1.0 - texture(skybox, TexCoords).z;

    FragColor.w = 1.0;
}