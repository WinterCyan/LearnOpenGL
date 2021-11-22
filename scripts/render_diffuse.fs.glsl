#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// material parameters
//uniform vec3 albedo;
//uniform float metallic;
//uniform float roughness;
uniform float ao;

// IBL, certain number(3)
uniform samplerCube irradianceMap; // 0
uniform samplerCube prefilterMap; // 1
uniform sampler2D brdfLUT; // 2

// material params with textures
uniform sampler2D albedoMap; // 4
//uniform sampler2D aiTextureType_DIFFUSE1; // 4
uniform sampler2D normalMap; // 5
uniform sampler2D metallicMap; // 6
uniform sampler2D roughnessMap; // 7
//uniform sampler2D aoMap;

// lights
uniform vec3 lightPositions[5];
uniform vec3 lightColors[5];

uniform vec3 camPos;

const float PI = 3.14159265359;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
    vec3 albedo = pow(texture(albedoMap,TexCoords).rgb,vec3(2.2));
    vec3 metallic = pow(texture(metallicMap,TexCoords).rgb, vec3(2.2));
    float roughness = texture(roughnessMap,TexCoords).r;
    vec3 N = getNormalFromMap();
    vec3 V = normalize(camPos - WorldPos);
    vec3 R = reflect(-V, N);

    vec3 kD = vec3(1.0) - metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 1.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (metallic * brdf.x + brdf.y);

    vec3 ambient = kD * diffuse + specular;

    vec3 color = ambient;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);

}
