#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;

out vec3 FragPos;
out vec3 FragNormal;

void main()
{
	gl_Position = u_proj * u_view * u_model * vec4(position, 1.0);

	FragPos = vec3(u_model * vec4(position, 1.0));

	FragNormal = mat3(transpose(inverse(u_model))) * normal;
}


#shader fragment
#version 330 core

uniform float u_innerCutoff;
uniform float u_outerCutoff;
uniform float u_intensity;

uniform vec3 u_objPos;
uniform vec3 u_lightPos;
uniform vec3 u_lightDir;
uniform vec3 u_cameraLocation;

in vec3 FragPos;
in vec3 FragNormal;

out vec4 color;

void main()
{
	float specularLobe = 64;
	float specularCoefficient = 0.7;
	float ambientStrength = 0.13;
	float diffuseCoefficient = 0.42;

	vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 objColor = vec4(0.2, 0.3, 0.8, 1.0);

	vec4 ambient = ambientStrength * lightColor;

	float distance = length(u_lightPos - FragPos);
	float energyArrived = u_intensity / (distance * distance);

	vec3 frag_to_light = normalize(u_lightPos - FragPos);

	float diff = max(
		0.0,
		dot(normalize(FragNormal), frag_to_light)
	);

	vec4 diffuse =
		diffuseCoefficient *
		energyArrived *
		diff *
		lightColor;

	vec3 eyeDir = normalize(u_cameraLocation - FragPos);

	vec3 bisector = normalize(eyeDir + frag_to_light);

	float reflectionLobe = max(
		0.0,
		dot(normalize(FragNormal), bisector)
	);

	vec4 specular =
		specularCoefficient *
		energyArrived *
		pow(reflectionLobe, specularLobe) *
		lightColor;

	vec4 originalColor =
		(ambient + diffuse + specular) *
		objColor;


	float theta = dot(
		normalize(u_lightDir),
		normalize(-frag_to_light)
	);

	float phi = cos(u_innerCutoff);

	if (theta > phi)
	{
		color = originalColor;
	}
	else
	{
		float gamma = cos(u_outerCutoff);

		if (theta > gamma)
		{
			float modified_intensity =
				(theta - gamma) / (phi - gamma);

			color = modified_intensity * originalColor;
		}
		else
		{
			color = ambient * objColor;
		}
	}
}