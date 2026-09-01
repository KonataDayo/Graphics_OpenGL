#shader vertex
#version 330 core
layout(location = 0)in vec3 aPos;

out vec3 worldPos;
uniform mat4 proj;
uniform mat4 view;

void main()
{
	worldPos = aPos;
	gl_Position = proj * view * vec4(worldPos, 1.0);
}


#shader fragment
#version 330 core


uniform samplerCube envMap;
in vec3 worldPos;
out vec4 fragColor;
const float PI = 3.14159265359;

void main()
{
	vec3 irradiance = vec3(0.0);
	vec3 normal = normalize(worldPos);
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, normal));
	up = normalize(cross(normal,right));

	// irradiance convolution
	float sampleDelta = 0.025;
	float sampleNum = 0.0;
	for(float phi = 0; phi < PI * 2.0; phi += sampleDelta)
	{
		for(float theta = 0; theta < PI * 0.5; theta += sampleDelta)
		{
			// spherical to cartesian
			vec3 v = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

			vec3 sampleVec = normalize(v.x * right + v.y * up + v.z * normal);

			irradiance += texture(envMap, sampleVec).rgb * cos(theta) * sin(theta);
			sampleNum++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(sampleNum));

	fragColor = vec4(irradiance ,1.0);
}