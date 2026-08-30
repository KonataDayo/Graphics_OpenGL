#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 u_Normal;
layout(location = 2) in vec2 aTexCoord;
uniform mat4 u_Model;
uniform mat4 u_MVP;
uniform mat4 lightSpaceMatrix;
out vec4 lightSpaceFragPos;
out vec3 u_FragPos;
out vec3 Normal;
out vec2 texCoord;
void main()
{
    texCoord = aTexCoord;
    u_FragPos = vec3(u_Model * vec4(position, 1.0));
    gl_Position = u_MVP * vec4(position.xyz,1.0);
    Normal = transpose(inverse(mat3(u_Model))) * u_Normal;
    lightSpaceFragPos = lightSpaceMatrix * vec4(u_FragPos, 1.0);
}

#shader fragment
#version 330 core

out vec4 color;

in vec3 u_FragPos;
in vec3 Normal;
in vec4 lightSpaceFragPos;
in vec2 texCoord;

uniform sampler2D u_Texture;
uniform sampler2D u_DepthMap;
uniform vec3 u_LightPosition;
uniform vec3 u_CameraLocation;

float calculateShadow(vec4 lightSpaceFragPos, vec3 lightDir)
{
    vec3 clippedCoord = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
    clippedCoord = clippedCoord * 0.5 + 0.5;
    if (clippedCoord.z > 1.0)
        return 0.0;
    float closestDepth = texture(u_DepthMap, clippedCoord.xy).r;
    float currentDepth = clippedCoord.z;
    float bias = max(0.005, 0.05 * (1 - dot(Normal,lightDir)));
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main()
{
    vec4 u_LightColor = vec4(1.0, 1.0, 1.0, 1.0);
    float u_AmbientStrength = 0.3;
    float u_DiffuseCoefficient = 0.4;
    float u_Intensity = 100.0;
    float u_SpecularCoefficient = 0.7;
    float u_SpecularSize = 64;
    
    vec4 ambient = u_AmbientStrength * u_LightColor;

    float distance = length(u_LightPosition - u_FragPos);
    float energyArrived = u_Intensity / (distance * distance);
    vec3 lightDir = normalize(u_LightPosition - u_FragPos);
    float diff = max(0.0, dot(normalize(Normal), lightDir));
    vec4 diffuse = u_DiffuseCoefficient * energyArrived * diff * u_LightColor;

    vec3 eyeDir = normalize(u_CameraLocation - u_FragPos);
    vec3 bisector = normalize(eyeDir + lightDir);

    float reflectionLobe = max(0.0, dot(normalize(Normal),bisector));

    vec4 specular = u_SpecularCoefficient * energyArrived * pow(reflectionLobe, u_SpecularSize) * u_LightColor;

    float shadow = calculateShadow(lightSpaceFragPos, lightDir);

    color = (ambient + (diffuse + specular) * (1.0 - shadow)) * vec4(texture(u_Texture, texCoord).rgb, 1.0);
}