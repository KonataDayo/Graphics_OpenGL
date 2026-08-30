#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec3 WorldPos;
out vec3 normal;
out vec2 TexCoord;

void main()
{
    WorldPos = vec3(model * vec4(aPos, 1.0));

    normal = mat3(transpose(inverse(model))) * aNormal;

    TexCoord = aTexCoord;

    gl_Position = proj * view * vec4(WorldPos, 1.0);
}

#shader fragment
#version 330 core

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform sampler2D albedo;
uniform sampler2D normalMap;
uniform sampler2D roughness;
uniform sampler2D metallic;

uniform vec3 cameraPos;

in vec3 WorldPos;
in vec3 normal;
in vec2 TexCoord;

out vec4 color;

const float PI = 3.14159265359;

// --------------------------------------------------
// Translate normal from Tangent space to world space
// --------------------------------------------------
vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoord).xyz * 2.0 - 1.0 ;

    // a = bT + cB
    // d = eT + fB 
    vec3 a = dFdx(WorldPos);
    vec3 d = dFdy(WorldPos);
    
    vec2 pdx = dFdx(TexCoord);
    vec2 pdy = dFdy(TexCoord);

    float b = pdx.s;
    float c = pdx.t;
    float e = pdy.s;
    float f = pdy.t;

    vec3 N = normalize(normal);
    vec3 T = normalize(f * a - c * d);
    // Gram-Schmidt
    T = normalize(T - dot(T, N) * N);
    // Cross product
    vec3 B = normalize(cross(N, T));

    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}


// --------------------------------------------------
// Fresnel Schlick
// --------------------------------------------------

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


// --------------------------------------------------
// GGX Normal Distribution Function
// --------------------------------------------------

float NDF_GGXTR(vec3 n, vec3 h, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(n, h), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denominator = PI * pow(NdotH2 * (a2 - 1.0) + 1.0, 2.0);

    return a2 / denominator;
}


// --------------------------------------------------
// Schlick-GGX Geometry Function
// --------------------------------------------------

float SchlickGGX(vec3 n, vec3 v, float roughness)
{
    float k = pow(roughness + 1.0, 2.0) / 8.0;
    float NdotV = max(dot(n, v), 0.0);

    return NdotV / (NdotV * (1.0 - k) + k);
}


// --------------------------------------------------
// Main
// --------------------------------------------------

void main()
{
    vec3 N = GetNormalFromMap();
    vec3 V = normalize(cameraPos - WorldPos);

    vec3 Albedo = texture(albedo, TexCoord).rgb;
    float Metallic = texture(metallic, TexCoord).r;
    float Roughness = texture(roughness, TexCoord).r;

    // Base reflectivity
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, Metallic);

    vec3 Lo = vec3(0.0);

    for (int i = 0; i < 4; i++)
    {
        // Light direction
        vec3 L = normalize(lightPositions[i] - WorldPos);

        // Distance and attenuation
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);

        // Incoming radiance
        vec3 Li = lightColors[i] * attenuation;

        // Halfway vector
        vec3 H = normalize(V + L);

        // Dot products
        float NdotL = max(dot(N, L), 0.0);
        float NdotV = max(dot(N, V), 0.0);
        float HdotV = max(dot(H, V), 0.0);

        // Fresnel
        vec3 F = FresnelSchlick(HdotV, F0);

        // Energy conservation
        vec3 kS = F;
        vec3 kD = (1.0 - kS) * (1.0 - Metallic);

        // Diffuse BRDF
        vec3 f_lambert = Albedo / PI;

        // Normal Distribution Function
        float D = NDF_GGXTR(N, H, Roughness);

        // Geometry term - Smith's method
        float G = SchlickGGX(N, V, Roughness) * SchlickGGX(N, L, Roughness);

        // Cook-Torrance specular BRDF
        vec3 Numerator = D * F * G;
        float Denominator = 4.0 * NdotV * NdotL + 0.00001;

        vec3 f_specular = Numerator / Denominator;

        // Complete BRDF
        vec3 BRDF = kD * f_lambert + f_specular;

        // Rendering equation
        Lo += BRDF * Li * NdotL;
    }
    color = vec4(Lo, 1.0);
}