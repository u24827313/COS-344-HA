#version 330 core
in  vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;

void main()
{

    FragColor.x = (texture(skybox, TexCoords).x * 0.393)
                 + (texture(skybox, TexCoords).y * 0.769)
                 + (texture(skybox, TexCoords).z * 0.189);

    FragColor.y = (texture(skybox, TexCoords).x * 0.349)
                 + (texture(skybox, TexCoords).y * 0.686)
                 + (texture(skybox, TexCoords).z * 0.168);

    FragColor.z = (texture(skybox, TexCoords).x * 0.272)
                 + (texture(skybox, TexCoords).y * 0.534)
                 + (texture(skybox, TexCoords).z * 0.131);

    FragColor.w = 1.0;
}