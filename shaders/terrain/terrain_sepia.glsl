#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 UV;
in vec3 WorldPos;

out vec4 FragColour;

// Lighting
uniform vec3  lightDirection;  
uniform vec3  lightColour;
uniform float lightAmbient;
uniform vec3  viewPos;

uniform sampler2D grassTex;
uniform sampler2D sandTex;
uniform sampler2D rockTex;
uniform sampler2D concreteTex;
uniform sampler2D woodTex;

uniform float tileFactor;

uniform float worldHalfSize;

#define MAX_BUNKERS 32
uniform int  numBunkers;
uniform vec4 bunkers[MAX_BUNKERS];


void computeWeights(out float grassW,
                    out float sandW,
                    out float rockW,
                    out float concreteW,
                    out float woodW)
{
    grassW = concreteW = woodW = 0.0;
    sandW = rockW = 0.0;

    // dot(normal, up) is 1.0 for flat ground, 0.0 for vertical cliffs.
    float upness   = clamp(dot(normalize(Normal), vec3(0.0, 1.0, 0.0)), 0.0, 1.0);
    float steepness = 1.0 - upness;
    
    rockW = smoothstep(0.35, 0.7, steepness);

    
    for (int i = 0; i < numBunkers; ++i) {
        vec2 center   = bunkers[i].xy;
        float radius  = bunkers[i].z;
        float fade    = bunkers[i].w;
        float d = distance(WorldPos.xz, center);
    
        float w = 1.0 - smoothstep(radius, radius + fade, d);
        sandW = max(sandW, w);
    }

    
    float distFromEdge = worldHalfSize - max(abs(WorldPos.x), abs(WorldPos.z));
    
    float borderStrength = 1.0 - smoothstep(5.0, 15.0, distFromEdge);
    
    float woodMix = smoothstep(-1.0, 1.0, sin(WorldPos.x * 0.05) + cos(WorldPos.z * 0.05));
    woodW += borderStrength * woodMix;
    rockW += borderStrength * (1.0 - woodMix);

    grassW = 1.0 - clamp(sandW + rockW + concreteW + woodW, 0.0, 1.0);

    
    float total = grassW + sandW + rockW + concreteW + woodW;
    if (total > 0.0001) {
        grassW    /= total;
        sandW     /= total;
        rockW     /= total;
        concreteW /= total;
        woodW     /= total;
    }
}

void main() {
    
    vec2 tiledUV = UV * tileFactor;

    
    vec3 grass    = texture(grassTex,    tiledUV).rgb;
    vec3 sand     = texture(sandTex,     tiledUV).rgb;
    vec3 rock     = texture(rockTex,     tiledUV).rgb;
    vec3 concrete = texture(concreteTex, tiledUV).rgb;
    vec3 wood     = texture(woodTex,     tiledUV).rgb;

    
    float gW, sW, rW, cW, wW;
    computeWeights(gW, sW, rW, cW, wW);

    
    vec3 baseColour = grass * gW + sand  * sW  + rock  * rW + concrete * cW + wood  * wW;


    vec3 N = normalize(Normal);
    
    vec3 L = normalize(-lightDirection);

    
    vec3 ambient = lightAmbient * lightColour;

    
    float diff   = max(dot(N, L), 0.0);
    vec3 diffuse = diff * lightColour;

    vec3 V = normalize(viewPos - FragPos);
    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(V, R), 0.0), 16.0);
    vec3 specular = 0.1 * spec * lightColour;

    vec3 lit = (ambient + diffuse + specular) * baseColour;

    FragColour.x = (lit.x * 0.393)
                 + (lit.y * 0.769)
                 + (lit.z * 0.189);

    FragColour.y = (lit.x * 0.349)
                 + (lit.y * 0.686)
                 + (lit.z * 0.168);

    FragColour.z = (lit.x * 0.272)
                 + (lit.y * 0.534)
                 + (lit.z * 0.131);

    FragColour.w = 1.0;
}