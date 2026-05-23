#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 UV;

out vec4 FragColour;

uniform sampler2D diffuseTex;
uniform vec3  lightDirection;
uniform vec3  lightColour;
uniform float lightAmbient;
uniform vec3  viewPos;

void main() {
    vec3 base = texture(diffuseTex, UV).rgb;

    vec3 N = normalize(Normal);
    vec3 L = normalize(-lightDirection);
    vec3 ambient  = lightAmbient * lightColour;
    float diff    = max(dot(N, L), 0.0);
    vec3 diffuse  = diff * lightColour;
    vec3 V = normalize(viewPos - FragPos);
    vec3 R = reflect(-L, N);
    float spec    = pow(max(dot(V, R), 0.0), 32.0);
    vec3 specular = 0.2 * spec * lightColour;

    vec4 pre = vec4((ambient + diffuse + specular) * base, 1.0);

    FragColour.x = (pre.x * 0.393)
                 + (pre.y * 0.769)
                 + (pre.z * 0.189);

    FragColour.y = (pre.x * 0.349)
                 + (pre.y * 0.686)
                 + (pre.z * 0.168);

    FragColour.z = (pre.x * 0.272)
                 + (pre.y * 0.534)
                 + (pre.z * 0.131);

    FragColour.w = 1.0;
}