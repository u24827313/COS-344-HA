#version 330 core
in  vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;

void main()
{

    FragColor.x = texture(skybox, TexCoords).y;

    FragColor.y = texture(skybox, TexCoords).z;

    FragColor.z = texture(skybox, TexCoords).x;

    FragColor.w = 1.0;
}