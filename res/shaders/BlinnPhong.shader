#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 u_Normal;
uniform mat4 u_Model;
uniform mat4 u_MVP;
out vec3 u_FragPos;
out vec3 Normal;
void main()
{
    u_FragPos = vec3(u_Model * vec4(position, 1.0));
    gl_Position = u_MVP * vec4(position.xyz,1.0);
    Normal = mat3(transpose(inverse(u_Model))) * u_Normal;

}

#shader fragment
#version 330 core

out vec4 color;

in vec3 u_FragPos;
in vec3 Normal;

uniform vec4 u_LightColor;
uniform vec4 u_ObjColor;

uniform float u_AmbientStrength;
uniform float u_DiffuseCoefficient;

uniform vec3 u_LightPosition;
uniform float u_Intensity;

uniform float u_SpecularCoefficient;
uniform float u_SpecularSize;
uniform vec3 u_CameraLocation;

void main()
{
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

    color = (ambient + diffuse + specular) * u_ObjColor;
}