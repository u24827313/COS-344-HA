#version 330 core
in  vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;

void main()
{

    FragColor.x = texture(skybox, TexCoords).z;

    FragColor.y = texture(skybox, TexCoords).x;

    FragColor.z = texture(skybox, TexCoords).y;

    FragColor.w = 1.0;
}